#include <Multiplayer.hpp>
#include <Logger.hpp>
#include <Player.hpp>
#include <Game.hpp>
#include <Chunk.hpp>
#include <thread>
#include <Debug.hpp>
#include <sockpp/platform.h>
#include <OnlinePlayer.hpp>

using Header = SerializedObject::Header;

Multiplayer::Multiplayer() : PacketManager() {}

bool Multiplayer::connect(std::string const& host, in_port_t port) {
    if (auto res = m_sock.connect(host, port); !res) {
        logE("Failed to connect to {}:{}: {}", host, port, res.error_message());
        return false;
    }

    logD("Connected!");

    auto game = Game::get();
    auto myUsername = game->getUsername();
    auto player = game->getPlayer();

    auto packet = GamePacket(SerializedObject::Header::IDENTIFICATION);
    packet.add(myUsername);
    if(send(packet)) {
        logD("Identification sent. Waiting for response...");
    }

    auto response = recv();
    if(!response.size()) {
        logD("Failed to get response :(");
        return false;
    }

    auto header = response.get<SerializedObject::Header>();
    if(header == Header::NETWORK_ERROR) handleError(response);
    if(header != Header::IDENTIFICATION) return false;

    m_myPlayerId = response.get<PlayerID>(0);
    logD("Received PlayerID from server {}", m_myPlayerId);
    player->setID(m_myPlayerId);

    m_connected = true;

    std::thread(&Multiplayer::outThread, this).detach();
    std::thread(&Multiplayer::inThread, this).detach();

    return true;
}

bool Multiplayer::connected() {
    return m_connected;
} 

void Multiplayer::onBlockChanged(Vec2i pos, uint8_t layer) {
    auto block = Game::get()->getWorld()->getBlock(pos.x, pos.y, layer);

    if(block) {
        addToQueue(std::shared_ptr<Block>(block));
    }
}

void Multiplayer::inThread() {
    while(m_connected) {
        // std::this_thread::sleep_for(std::chrono::milliseconds(8));
        auto read = recv();
        if(!read.get<Header>()) {
            m_connected = false;
            return logE("Connection lost {}", read.get<std::string>());
        }

        read.reset();
        
        auto packet = GamePacket(read);
        handle(packet);
    }
}

void Multiplayer::outThread() {
    while(m_connected) {
        std::this_thread::sleep_for(std::chrono::milliseconds(17));
        
        if(m_queue.empty()) {
            send(GamePacket(Header::KEEP_ALIVE));
            continue;
        }

        sendQueue();
    }
}

void Multiplayer::handle(GamePacket& packet) {
    PacketManager::handle(packet);

    switch(packet.get<Header>()) {
        case Header::UNLOAD_PLAYER: handleUnloadPlayer(packet); break;
        case Header::LOAD_PLAYER: handleLoadPlayer(packet); break;
        case Header::NETWORK_ERROR: handleError(packet); break;
        case Header::PLAYER: handlePlayer(packet); break;
        case Header::CHUNK: handleChunk(packet); break;
        case Header::BLOCK: handleBlock(packet); break;
        default: return;
    }
}

void Multiplayer::handleError(GamePacket& packet) {
    logE("Server error! {}", packet.get<std::string>("unknown"));
    std::exit(1);
}

void Multiplayer::handlePlayer(GamePacket& packet) {
    auto game = Game::get();
    auto world = game->getWorld();
    auto me = game->getPlayer();
    auto id = packet.get<PlayerID>(0);

    if(id > 0) {
        if(id == me->getID()) {
            me->deserialize(packet.bytes());
            return;
        }

        auto otherPlayer = world->getPlayer(id);

        if(!otherPlayer) {
            otherPlayer = std::make_shared<OnlinePlayer>(world);
            world->addPlayer(id, otherPlayer);
        }

        if(otherPlayer->getUsername().empty()) {
            addToQueue(CREATE_PACKET(SerializedObject::LOAD_PLAYER, id));
        }

        otherPlayer->deserialize(packet.bytes());
    }
}

void Multiplayer::handleLoadPlayer(GamePacket& packet) {
    auto game = Game::get();
    auto world = game->getWorld();
    auto id = packet.get<PlayerID>(0);
    logD("Load player {}", id);

    if(id > 0) {
        auto player = world->getPlayer(id);
        auto username = packet.get<std::string>("undefined");
        
        if(!player) {
            player = std::make_shared<OnlinePlayer>(world);
            world->addPlayer(id, player);
        }

        player->setUsername(username);
    }
}

void Multiplayer::handleUnloadPlayer(GamePacket& packet) {
    auto game = Game::get();
    auto world = game->getWorld();
    auto id = packet.get<PlayerID>(0);
    logD("Unload player {}", id);

    if(world->getPlayer(id)) {
        world->unloadPlayer(id);
    }
}

void Multiplayer::handleChunk(GamePacket& packet) {
    auto game = Game::get();
    auto world = game->getWorld();
    auto chunk = std::make_shared<Chunk>(world);
    chunk->deserialize(packet.bytes());

    logD("Received chunk {}", chunk->getPosition());

    world->addChunk(chunk);

    if(world->getPlayer(m_myPlayerId) == nullptr) {
        auto player = game->getPlayer();
        world->addPlayer(m_myPlayerId, player);
    }
}

void Multiplayer::handleBlock(GamePacket& packet) {
    auto game = Game::get();
    
    auto block = std::make_shared<Block>();
    block->deserialize(packet.bytes());

    auto pos = block->getPos();
    auto lay = block->getLayer();

    game->getWorld()->setBlock(pos.x, pos.y, lay, block);
    
    logD("Received block {}, {}, {}", pos.x, pos.y, lay);
}