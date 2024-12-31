#include <Multiplayer.hpp>
#include <Logger.hpp>
#include <Player.hpp>
#include <Game.hpp>
#include <Chunk.hpp>
#include <thread>
#include <Debug.hpp>
#include <sockpp/platform.h>
using namespace std::chrono;

#if defined(_WIN32)
  #define WOULDBLOCK WSAEWOULDBLOCK
#elif defined(__linux__)
  #define WOULDBLOCK EWOULDBLOCK
#else
  #error Unknown platform WOULDBLOCK error code
#endif

using Header = SerializedObject::Header;

Multiplayer::~Multiplayer() {
    m_connector.shutdown();
    delete m_pacman;
}

bool Multiplayer::connect(std::string const& host, in_port_t port) {
    if (auto res = m_connector.connect(host, port); !res) {
        logE("Failed to connect to {}:{}: {}", host, port, res.error_message());
        return false;
    }

    m_pacman = new PacketManager(m_connector, MP_BUF_SIZE);

    auto game = Game::get();
    auto myUsername = game->getUsername();
    auto player = game->getPlayer();

    auto packet = std::make_shared<GamePacket>(SerializedObject::Header::IDENTIFICATION);
    packet->addBytes(myUsername);

    m_pacman->send(packet);

    auto response = m_pacman->recv();
    if(!response->getSize()) return false;

    m_connector.set_non_blocking();

    auto header = response->getBytes<SerializedObject::Header>();
    if(header == SerializedObject::Header::NETWORK_ERROR) {
        logE("SERVER ERROR {}", response->getBytes<std::string>());
        std::exit(1);
    }

    auto id = response->getBytes<PlayerID>(0);
    player->setID(id);
    game->getWorld()->addPlayer(id, player);

    logD("My playerID {}", id);

    return true;
}

void Multiplayer::addToQueue(std::shared_ptr<SerializedObject> classObj) {
    if(std::find(m_queue.begin(), m_queue.end(), classObj) != m_queue.end()) return;
    if(m_queue.size() > 10) logD("big queue size {}", m_queue.size());

    m_queue.push_back(classObj);
}

void Multiplayer::onBlockChanged(Vec2i pos, uint8_t layer) {
    auto block = Game::get()->getWorld()->getBlock(pos.x, pos.y, layer);
    if(block) {
        logD("block changed {}", (int)block->getID());
        addToQueue(std::shared_ptr<Block>(block));
    }
}

void Multiplayer::onTick() {
    auto game = Game::get();
    auto world = game->getWorld();
    auto me = game->getPlayer();

    // packet queue is not empty and world loaded
    if(!m_queue.empty() && m_worldLoaded && m_pacman->send(CREATE_PACKET((*m_queue.begin())->serialize()))) {
        m_queue.erase(m_queue.begin());
        return;
    }

    auto packet = m_pacman->recv();
    if(!packet) return;

    switch (packet->getBytes<Header>()) {
        case Header::NETWORK_ERROR: {
            logE("SERVER ERROR {}", packet->getBytes<std::string>());
            std::exit(1);
            break;
        }

        case Header::LOAD_PLAYER: {
            auto id = packet->getBytes<PlayerID>(0);
            logD("Load player {}", id);
            if(!id) break;
            
            auto player = world->getPlayer(id);
            auto username = packet->getBytes<std::string>("undefined");
            
            if(!player) {
                player = std::make_shared<SimplePlayer>(world);
                world->addPlayer(id, player);
            }

            player->setUsername(username);

            break;
        }

        case Header::UNLOAD_PLAYER: {
            auto id = packet->getBytes<PlayerID>(0);
            logD("Unload player {}", id);
            if(!world->getPlayer(id)) break;
            
            world->unloadPlayer(id);
            
            break;
        }

        case Header::PLAYERS: {
            auto count = packet->getBytes<uint32_t>(0);
            if(packet->getSize() > 5 + SimplePlayer::getSizeBytes() * count) {
                logD("STRANGE PACKET {} BYTES ", packet->getSize());
                for(auto i = 0; i < packet->getSize() - 5; i++) std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)packet->getBytes<uint8_t>() << " ";
                std::cout << std::endl;
            }
            
            while(count-- > 0) {
                auto bytes = packet->getBytes(SimplePlayer::getSizeBytes());
                auto playerPacket = CREATE_PACKET(bytes);
                
                if(playerPacket->getBytes<SerializedObject::Header>() != SerializedObject::PLAYER) continue;
                auto id = playerPacket->getBytes<PlayerID>(0);
                if(!id) continue;

                // logD("Update player id {}", id);

                auto player = world->getPlayer(id);
                if(!player) {
                    player = std::make_shared<SimplePlayer>(world);
                    world->addPlayer(id, player);
                }

                if(player->getUsername().empty()) addToQueue(CREATE_PACKET(SerializedObject::LOAD_PLAYER, id));

                player->deserialize(bytes);
            }
            break;
        }

        case Header::PLAYER: {
            auto id = packet->getBytes<PlayerID>(0);
            if(id == 0) {
                break;
            }

            if(id == me->getID()) {
                me->deserialize(packet->serialize());
                break;
            }

            auto otherPlayer = world->getPlayer(id);
            if(!otherPlayer) {
                otherPlayer = std::make_shared<SimplePlayer>(world);
                world->addPlayer(id, otherPlayer);
            }

            if(otherPlayer->getUsername().empty()) addToQueue(CREATE_PACKET(SerializedObject::LOAD_PLAYER, id));

            otherPlayer->deserialize(packet->serialize());

            break;
        }

        case Header::BLOCK: {
            auto block = std::make_unique<Block>(Block::ID::AIR);
            block->deserialize(packet->serialize());
            auto pos = block->getPos();
            auto layer = block->getLayer();

            world->setBlock(pos.x, pos.y, layer, std::move(block));

            break;
        }

        case Header::CHUNK: {
            auto chunk = std::make_shared<Chunk>(world);
            chunk->deserialize(packet->serialize());

            logD("Chunk {} ({} bytes)", chunk->getPosition(), packet->serialize().size());

            world->addChunk(chunk);
            break;
        }

        case Header::CHUNKS: {
            auto chunksCount = packet->getBytes<uint8_t>();

            while(chunksCount-- > 0) {
                auto chunkSize = packet->getBytes<uint16_t>();
                auto chunkBytes = packet->getBytes((size_t)chunkSize);
                auto chunk = std::make_shared<Chunk>(world);
                chunk->deserialize(chunkBytes);
                world->addChunk(chunk);
            }

            m_worldLoaded = true;

            break;
        }

        default: {
            return;
        }
    }
}