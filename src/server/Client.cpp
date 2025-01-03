#include <Client.hpp>
#include <Server.hpp>
#include <World.hpp>
#include <Chunk.hpp>
#include <Logger.hpp>
#include <Timer.hpp>
#include <mutex>

Client::Client(sockpp::tcp_socket sock) : PacketManager(std::move(sock)) {}

bool Client::accept() {
    auto srv = Server::get();

    auto identification = recv();
    if(!identification.size()) {
        logE("Failed to accept player (can't receive packet)");
        return false;
    }

    auto head = identification.get<Header>();
    
    if(head != Header::IDENTIFICATION) {
        logE("Failed to accept player (invalid first packet)");
        send(GamePacket(Header::NETWORK_ERROR, "First packet should be identification!"));
        return false;
    }

    auto username = identification.get<std::string>();

    if(username.size() < 3) {
        send(GamePacket(Header::NETWORK_ERROR, "Too short username!"));
        return false;
    }

    if(username.length() > 16) {
        logE("Failed to accept player (too long username)");
        send(GamePacket(Header::NETWORK_ERROR, "Too long username!"));
        return false;
    }

    if(srv->getWorld()->isUsernameAlreadyTaken(username)) {
        logE("Failed to accept player (username already taken)");
        send(GamePacket(Header::NETWORK_ERROR, "Username already taken!"));
        return false;
    }

    m_id = srv->joinPlayer(username);
    send(GamePacket(Header::IDENTIFICATION, m_id));

    uint16_t chunksCount = 3;
    auto worldpak = GamePacket(Header::ARRAY, chunksCount);

    while(chunksCount-- > 0) {
        auto bytes = srv->getWorld()->getChunk(chunksCount)->serialize();
        worldpak.add<uint16_t>(bytes.size());
        worldpak.add(bytes);
    }

    logD("Sending world to player {}...", username);
    send(worldpak);

    for(auto& [id, player] : srv->getWorld()->getPlayers()) {
        if(player->getID() == id) continue;
        addToQueue(player);
    }

    std::thread(&Client::inThread, this).detach();
    std::thread(&Client::outThread, this).detach();

    return true;
}

void Client::inThread() {
    while(!m_shouldDisconnect) {
        auto read = recv();
        if(!read.get<Header>()) {
            m_shouldDisconnect = true;
            return logE("Connection lost {}", read.get<std::string>());
        }
        read.reset();
        
        auto packet = GamePacket(read);
        handle(packet);
    }
}

void Client::outThread() {
    auto srv = Server::get();

    while(!m_shouldDisconnect) {
        sendQueue();

        std::this_thread::sleep_for(std::chrono::milliseconds(17));
    }
}

void Client::handle(GamePacket& packet) {
    PacketManager::handle(packet);

    switch(packet.get<Header>()) {
        case Header::LOAD_PLAYER: handleLoadPlayer(packet); break;
        case Header::PLAYER: handlePlayer(packet); break;
        case Header::BLOCK: handleBlock(packet); break;
        default: break;
    }
}

void Client::handlePlayer(GamePacket& packet) {
    packet.reset();
    
    auto srv = Server::get();
    
    auto player = srv->getWorld()->getPlayer(m_id); 
    player->deserialize(packet.bytes());
    srv->addToQueueExcept(m_id, player);
}

void Client::handleLoadPlayer(GamePacket& packet) {
    auto srv = Server::get();
    auto id = packet.get<PlayerID>(0);
    auto player = srv->getWorld()->getPlayer(id);

    if(player != nullptr) {
        auto packet = CREATE_PACKET(Header::LOAD_PLAYER, id);
        packet->add(player->getUsername());
        return addToQueue(packet);
    }

    return addToQueue(CREATE_PACKET(Header::UNLOAD_PLAYER, id));
}

void Client::handleBlock(GamePacket& packet) {
    auto srv = Server::get();
    
    auto block = std::make_shared<Block>();
    block->deserialize(packet.bytes());

    auto pos = block->getPos();
    auto lay = block->getLayer();

    srv->getWorld()->setBlock(pos.x, pos.y, lay, block);
    
    logD("Player changed block {}, {}, {}", pos.x, pos.y, lay);
    srv->addToQueueExcept(m_id, block);
}