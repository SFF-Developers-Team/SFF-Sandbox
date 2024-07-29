#include <Multiplayer.hpp>
#include <Logger.hpp>
#include <Player.hpp>
#include <Game.hpp>
#include <Chunk.hpp>
#include <thread>
#include <Debug.hpp>

using namespace std::chrono;

bool Multiplayer::connect(std::string const& host, in_port_t port) {
    if (auto res = m_connector.connect(host, port); !res) {
        logE("Failed to connect to {}:{}: {}", host, port, res.error_message());
        return false;
    }

    auto game = Game::get();
    auto myUsername = game->getUsername();
    auto player = game->getPlayer();

    auto packet = new GamePacket(SerializedObject::Header::IDENTIFICATION);
    packet->addBytes(myUsername);
    send(packet);

    auto response = read(512);
    if(!response->getSize()) return false;

    m_connector.set_non_blocking();

    auto header = response->getBytes<SerializedObject::Header>();

    if(header == SerializedObject::Header::SERVER_ERROR) {
        logE("SERVER ERROR {}", packet->getBytes<std::string>());
        std::exit(1);
    }

    auto id = response->getBytes<PlayerID>(0);
    
    logD("player id {}", id);

    player->setID(id);
    game->getWorld()->addPlayer(id, player);

    std::thread thr(&Multiplayer::thread, this);
    thr.detach();

    delete response;
    return true;
}

void Multiplayer::send(GamePacket* packet) {
    auto bytes = packet->serialize();
    m_connector.write(bytes.data(), bytes.size());

    delete packet;
}

GamePacket* Multiplayer::read() {
    return read(MP_BUF_SIZE);
}

GamePacket* Multiplayer::read(size_t n) {
    auto buf = ByteVector(n);
    return read(buf);
}

GamePacket* Multiplayer::read(ByteVector& buf) {
    auto result = m_connector.read(buf.data(), buf.size());
    // Ееее рок хардкод (10035 - EWOULDLOCK)
    if(result.is_error() && result.error().value() == 10035) {
        return new GamePacket(SerializedObject::Header::NULL_PACKET); 
    }

    return (result.is_ok() ? new GamePacket(buf) : nullptr);
}

void Multiplayer::addToQueue(GamePacket* packet) {
    m_packetQueue.push_back(packet);
}

void Multiplayer::thread() {
    static ByteVector buf(MP_BUF_SIZE);
    auto game = Game::get();
    auto world = game->getWorld();
    auto player = game->getPlayer();

    while (true) {
        if(m_packetQueue.size() > 0) {
            send(*m_packetQueue.begin());
            m_packetQueue.erase(m_packetQueue.begin());
        }

        auto packet = read(buf);
        if(!packet) break;

        switch (packet->getBytes<SerializedObject::Header>()) {
            case SerializedObject::Header::SERVER_ERROR: {
                logE("SERVER ERROR {}", packet->getBytes<std::string>());
                std::exit(1);
                break;
            }

            case SerializedObject::Header::LOAD_PLAYER: {
                auto id = packet->getBytes<PlayerID>(-1);
                logD("Load player {}", id);
                if(id == -1 || world->getPlayer(id)) break;

                auto username = packet->getBytes<std::string>("undefined");
                auto player = new SimplePlayer(world, false);
                player->setUsername(username);
                world->addPlayer(id, player);
                break;
            }

            case SerializedObject::Header::UNLOAD_PLAYER: {
                auto id = packet->getBytes<PlayerID>(-1);
                if(id == -1 || !world->getPlayer(id)) break;
                
                world->unloadPlayer(id);
                break;
            }

            case SerializedObject::Header::PLAYER_POSITION: {
                auto id = packet->getBytes<PlayerID>(-1);
                auto pos = packet->getBytes<Vec2f>({0, 0});
                auto player = world->getPlayer(id);
                if(id == -1 || !player) break;

                player->setPosition(pos);
                break;
            }

            case SerializedObject::Header::PLAYER_ANIMATION: {
                auto id = packet->getBytes<PlayerID>(-1);
                auto frame = packet->getBytes<uint8_t>(0);
                auto player = world->getPlayer(id);
                if(id == -1 || !player) break;

                player->setAnimCurrentFrame(frame);
                break;
            }

            case SerializedObject::Header::PLAYER_DIRECTION: {
                auto id = packet->getBytes<PlayerID>(-1);
                auto dir = packet->getBytes<Player::Direction>(Player::Direction::RIGHT);
                auto player = world->getPlayer(id);
                if(id == -1 || !player) break;

                player->turn(dir);
                break;
            }

            case SerializedObject::Header::BLOCK: {
                auto block = new Block(Block::BlockType::AIR);
                block->deserialize(buf);
                auto pos = block->getPosition();

                world->setBlock(pos.x, pos.y, block->getLayer(), block);
                break;
            }

            case SerializedObject::Header::CHUNK: {
                auto chunk = new Chunk(world);
                chunk->deserialize(buf);

                world->setChunk(chunk);
                break;
            }
        }

        delete packet;
    }

    m_connector.set_non_blocking(false); // for future identifications
    m_connector.shutdown();
}