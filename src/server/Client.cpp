#include <Client.hpp>
#include <Server.hpp>
#include <world/World.hpp>
#include <world/Chunk.hpp>
#include <entity/SimplePlayer.hpp>
#include <Logger.hpp>
#include <Timer.hpp>
#include <mutex>

Client::Client(ENetPeer* peer) : PacketManager(peer) {}

bool Client::accept(Packet& packet) {
    auto srv = Server::get();
    auto head = packet.get<ObjectHeader>();
    
    if (head != ObjectHeader::IDENTIFICATION) {
        disconnect(INVALID_FIRST_PACKET);
        return false;
    }

    std::string username = packet.get("Player_" + std::to_string(rand() % 10000));

    if (username.size() < 3) {
        disconnect(TOO_SHORT_USERNAME);
        return false;
    }

    if (username.length() > 16) {
        disconnect(TOO_LONG_USERNAME);
        return false;
    }

    if (srv->getWorld()->isUsernameAlreadyTaken(username)) {
        disconnect(USERNAME_ALREADY_TAKEN);
        return false;
    }

    m_id = srv->joinPlayer(username);
    sendPacket(Packet(ObjectHeader::IDENTIFICATION, m_id));

    m_loggedIn = true;
    
    return true;
}

void Client::packetReceived(Packet& packet) {
    if(m_disconnect) {
        return;
    }

    if(!m_loggedIn && !accept(packet)) {
        return;
    }

    handle(packet);
}

void Client::disconnect(DisconnectReasonID reason) {
    auto srv = Server::get();
    logD("Disconnecting client {} because of {}", m_id, srv->getDisconnectReasonByID(reason));
    sendPacket(Packet(ObjectHeader::NETWORK_ERROR, srv->getDisconnectReasonByID(reason)));

    enet_peer_disconnect_later(m_peer, reason);
    m_disconnect = true;
}

void Client::handle(Packet& packet) {
    PacketManager::handle(packet);

    switch (packet.get<ObjectHeader>()) {
        case ObjectHeader::LOAD_PLAYER: return handleLoadPlayer(packet);
        case ObjectHeader::LOAD_CHUNK: return handleLoadChunk(packet);
        case ObjectHeader::BLOCK_PLACE: return handleBlockPlace(packet);
        case ObjectHeader::BLOCK_DESTROY: return handleBlockDestroy(packet);
        case ObjectHeader::PLAYER: return handlePlayer(packet);
        case ObjectHeader::LOAD_TERRAIN: return handleLoadTerrain(packet);
        case ObjectHeader::LOAD_PLAYERS: return handleLoadPlayers(packet);
        case ObjectHeader::LOAD_MESSAGE: return handleLoadMessages(packet);
        case ObjectHeader::MESSAGE: return handleMessage(packet);
        default: break;
    }
}

void Client::handleMessage(Packet& packet){
    auto srv = Server::get();
    auto player = srv->getWorld()->getPlayer(m_id);
    std::string msg = packet.get("");
    std::string msgFull = std::format("{}: {}", player->getUsername(), msg);
    

    logM("{}", msgFull);

    srv->pushMessage(msgFull);
    srv->broadcast(Packet(ObjectHeader::MESSAGE, msgFull));

    if(srv->getMessage().size() > 30) {
        srv->clearMessage();
    }
}

void Client::handleLoadMessages(Packet& packet) {
    auto srv = Server::get();
    auto messages = srv->getMessage();

    if(messages.size() > 0) {
        logD("Trying to send... ");
        sendPacket(Packet(ObjectHeader::MESSAGE, messages));
    }
}
void Client::handlePlayer(Packet& packet) {
    if(packet.get<PlayerID>() != m_id) {
        return;
    }

    packet.reset();

    auto srv = Server::get();
    auto player = srv->getWorld()->getPlayer(m_id);
    player->deserialize(packet);
    srv->broadcastExcept(m_id, std::shared_ptr<Serializable>(player), EVERYTHING, false);
}

void Client::handleLoadPlayer(Packet& packet) {
    auto srv = Server::get();
    auto id = packet.get<PlayerID>(0);
    auto player = srv->getWorld()->getPlayer(id);

    if (player != nullptr) {
        auto packet = Packet(ObjectHeader::LOAD_PLAYER);
        packet.add(id);
        packet.add(player->getUsername());
        
        sendPacket(packet, NOTIFICATIONS);
        return;
    }

    sendPacket(Packet(ObjectHeader::UNLOAD_PLAYER, id), Channel::NOTIFICATIONS);
}

void Client::handleBlockPlace(Packet& packet) {
    // auto srv = Server::get();
    // auto blockBytes = DataStream();
    // blockBytes.deserialize(packet.getN(packet.size() - 1));
    // auto block = Block::create(blockBytes);
    // auto pos = block->getPos();
    // auto lay = block->getLayer();

    // srv->getWorld()->setBlock(pos.x, pos.y, lay, block);

    // logD("Player changed block {}, {}, {}", pos.x, pos.y, lay);
    // srv->broadcast(packet);
}

void Client::handleBlockDestroy(Packet& packet) {
    auto x = packet.get<int32_t>();
    auto y = packet.get<int32_t>();
    auto l = packet.get<uint8_t>();
    auto srv = Server::get();
    srv->getWorld()->destroyBlock(x, y, l);
    srv->broadcast(packet);
}

void Client::handleLoadChunk(Packet& packet) {
    auto srv = Server::get();
    auto world = srv->getWorld();
    auto pos = packet.get<ChunkPosition>();
    auto chunk = world->getChunk(pos);

    if (chunk == nullptr) {
        chunk = world->getGenerator()->generateChunk(pos);
        world->addChunk(chunk);
    }

    sendObj(chunk);
}

void Client::handleLoadTerrain(Packet& packet) {
    auto world = Server::get()->getWorld();
    auto player = world->getPlayer(m_id);
    auto playerChunk = world->xToChunk(player->getHitbox().x - 1);
    auto terrain = Packet(ObjectHeader::TERRAIN, ObjectHeader::ARRAY, 3);

    for (auto i = playerChunk; i <= playerChunk + 2; i++) {
        auto chunk = world->getChunk(i)->serialize();
        terrain.add<uint16_t>(chunk.size());
        terrain.add(chunk);
    }

    sendPacket(terrain);
}

void Client::handleLoadPlayers(Packet& packet) {
    auto srv = Server::get();

    for (auto& [id, player] : srv->getWorld()->getPlayers()) {
        if (id != m_id) {
            sendPacket(player->serialize(), Channel::EVERYTHING);
            sendPacket(Packet(ObjectHeader::LOAD_PLAYER, id, player->getUsername()), Channel::NOTIFICATIONS);
        }
    }
}