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

Multiplayer::~Multiplayer() {
    m_connector.shutdown();
    delete m_pacman;
}

bool Multiplayer::connect(std::string const& host, in_port_t port) {
    if (auto res = m_connector.connect(host, port); !res) {
        logE("Failed to connect to {}:{}: {}", host, port, res.error_message());
        return false;
    }

    m_pacman = new PacketManager(MP_BUF_SIZE);

    auto game = Game::get();
    auto myUsername = game->getUsername();
    auto player = game->getPlayer();

    auto packet = std::make_shared<GamePacket>(SerializedObject::Header::IDENTIFICATION);
    packet->addBytes(myUsername);

    while(!m_pacman->send(m_connector, packet));

    auto response = m_pacman->recv(m_connector);
    while(response == nullptr) response = m_pacman->recv(m_connector);
    
    if(!response->getSize()) return false;

    m_connector.set_non_blocking();

    auto header = response->getBytes<SerializedObject::Header>();
    if(header == SerializedObject::Header::SERVER_ERROR) {
        logE("SERVER ERROR {}", packet->getBytes<std::string>());
        std::exit(1);
    }

    auto id = response->getBytes<PlayerID>(0);
    player->setID(id);
    game->getWorld()->addPlayer(id, player);

    for(int i = 0; i < Game::get()->getWorld()->getWidth() / CHUNK_WIDTH; i++) {
        addToQueue(CREATE_PACKET(SerializedObject::LOAD_CHUNK, i));
    }

    return true;
}

void Multiplayer::addToQueue(std::shared_ptr<GamePacket> packet) {
    m_packetQueue.push_back(packet);
    // logD("added packet {}", (int)packet->getBytes<SerializedObject::Header>());
}

void Multiplayer::onBlockChanged(Vec2i pos, uint8_t layer) {
    auto block = Game::get()->getWorld()->getBlock(pos.x, pos.y, layer);
    if(block) {
        addToQueue(CREATE_PACKET(block->serialize()));
    }
}

void Multiplayer::onTick() {
    static ByteVector buf(MP_BUF_SIZE);
    auto game = Game::get();
    auto world = game->getWorld();
    auto me = game->getPlayer(); // Player

    Debug::addString("Queue size: {}", m_packetQueue.size());

    if(m_packetQueue.empty()) {
        addToQueue(CREATE_PACKET(me->serialize()));
        m_canSendNext = true;
    }

    if(m_packetQueue.size() > 0 && m_canSendNext) {
        if(m_pacman->send(m_connector, *m_packetQueue.begin())) {
            m_packetQueue.erase(m_packetQueue.begin());
            m_canSendNext = false;
        }
    }


    auto packet = m_pacman->recv(m_connector);
    if(!packet) return;

    switch (packet->getBytes<SerializedObject::Header>()) {
        case SerializedObject::Header::SERVER_ERROR: {
            logE("SERVER ERROR {}", packet->getBytes<std::string>());
            std::exit(1);
            break;
        }

        case SerializedObject::Header::LOAD_PLAYER: {
            auto id = packet->getBytes<PlayerID>(0);
            logD("Load player {}", id);
            if(!id) break;
            
            auto player = world->getPlayer(id);
            auto username = packet->getBytes<std::string>("undefined");
            
            if(!player) {
                player = new SimplePlayer(world);
                world->addPlayer(id, player);
            }

            player->setUsername(username);
            break;
        }

        case SerializedObject::Header::UNLOAD_PLAYER: {
            auto id = packet->getBytes<PlayerID>(0);
            logD("Unload player {}", id);
            if(!world->getPlayer(id)) break;
            
            world->unloadPlayer(id);
            break;
        }

        case SerializedObject::Header::PLAYERS: {
            auto count = packet->getBytes<size_t>(0);
            // logD("Players count {}", count);

            for(auto i = 0; i < count; i++) {
                auto bytes = packet->getBytes(SimplePlayer::getSizeBytes());
                auto playerPacket = CREATE_PACKET(bytes);
                
                if(playerPacket->getBytes<SerializedObject::Header>() != SerializedObject::PLAYER) continue;
                auto id = playerPacket->getBytes<PlayerID>(0);
                if(!id) continue;

                // logD("PlayerID: {}", id);

                auto player = world->getPlayer(id);
                if(!player) {
                    player = new SimplePlayer(world);
                    world->addPlayer(id, player);
                }

                if(player->getUsername().empty()) addToQueue(CREATE_PACKET(SerializedObject::LOAD_PLAYER, id));

                player->deserialize(bytes);
            }

            break;
        }

        case SerializedObject::Header::PLAYER: {
            auto id = packet->getBytes<PlayerID>(0);

            if(id == me->getID()) {
                me->deserialize(packet->serialize());
                break;
            } else if(!id) break;

            auto otherPlayer = world->getPlayer(id);
            if(!otherPlayer) {
                otherPlayer = new SimplePlayer(world);
                world->addPlayer(id, otherPlayer);
            }

            if(otherPlayer->getUsername().empty()) addToQueue(CREATE_PACKET(SerializedObject::LOAD_PLAYER, id));

            otherPlayer->deserialize(packet->serialize());
            break;
        }

        case SerializedObject::Header::BLOCK: {
            auto block = std::make_unique<Block>(Block::BlockType::AIR);
            block->deserialize(packet->serialize());
            auto pos = block->getPosition();
            auto layer = block->getLayer();

            world->setBlock(pos.x, pos.y, layer, std::move(block));
            break;
        }

        case SerializedObject::Header::CHUNK: {
            auto chunk = new Chunk(world);
            chunk->deserialize(buf);

            logD("Chunk {}", chunk->getPosition());

            world->setChunk(chunk);
            break;
        }

        case SerializedObject::Header::NULL_PACKET: { return; }
    }

    m_canSendNext = true;
}