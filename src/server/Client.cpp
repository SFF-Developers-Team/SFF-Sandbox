#include <Client.hpp>
#include <Server.hpp>
#include <world/World.hpp>
#include <world/Chunk.hpp>
#include <Logger.hpp>
#include <Timer.hpp>
#include <mutex>

Client::Client(ENetPeer* peer) : PacketManager(peer) {}

bool Client::accept(Packet& packet) {
    auto srv = Server::get();
    auto head = packet.get<Header>();
    auto players = srv->getWorld()->getPlayers();
    
    if (head != Header::IDENTIFICATION) {
        disconnect(INVALID_FIRST_PACKET);
        return false;
    }

    std::string username = packet.get("Undefined");
    
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
    auto iden = Packet(Header::IDENTIFICATION, m_id);
    sendPacket(iden);

    uint16_t chunksCount = 3;

    while (chunksCount-- > 0) {
        sendObj(srv->getWorld()->getChunk(chunksCount));
    }

    for (auto& [id, player] : srv->getWorld()->getPlayers()) {
        if (id != m_id) {
            

            sendPacket(player->serialize(), Channel::EVERYTHING);
            sendPacket(Packet(Header::LOAD_PLAYER, id, player->getUsername()), Channel::NOTIFICATIONS);
        }
    }

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
    sendPacket(Packet(Header::NETWORK_ERROR, srv->getDisconnectReasonByID(reason)));

    enet_peer_disconnect_later(m_peer, reason);
    m_disconnect = true;
}

void Client::handle(Packet& packet) {
    PacketManager::handle(packet);

    switch (packet.get<Header>()) {
    case Header::LOAD_PLAYER:
        handleLoadPlayer(packet);
        break;
    case Header::LOAD_CHUNK:
        handleLoadChunk(packet);
        break;
    case Header::BLOCK_PLACE:
        handleBlockPlace(packet);
        break;
    case Header::BLOCK_DESTROY:
        handleBlockDestroy(packet);
        break;
    case Header::PLAYER:
        handlePlayer(packet);
        break;
    // case Header::BLOCK:
    //     handleBlock(packet);
    //     break;
    default:
        break;
    }
}

void Client::handlePlayer(Packet& packet) {
    if(packet.get<PlayerID>() != m_id) {
        return;
    }

    packet.reset();

    auto srv = Server::get();
    auto player = srv->getWorld()->getPlayer(m_id);
    player->deserialize(packet.bytes());
    srv->broadcastExcept(m_id, std::shared_ptr<SerializedObject>(player), EVERYTHING, false);
}

void Client::handleLoadPlayer(Packet& packet) {
    auto srv = Server::get();
    auto id = packet.get<PlayerID>(0);
    auto player = srv->getWorld()->getPlayer(id);

    if (player != nullptr) {
        auto packet = Packet(Header::LOAD_PLAYER);
        packet.add(id);
        packet.add(player->getUsername());
        
        sendPacket(packet, NOTIFICATIONS);
        return;
    }

    sendPacket(Packet(Header::UNLOAD_PLAYER, id), Channel::NOTIFICATIONS);
}

void Client::handleBlockPlace(Packet& packet) {
    auto srv = Server::get();

    auto block = std::make_shared<Block>();
    block->deserialize(packet.getN(packet.size() - 1));

    auto pos = block->getPos();
    auto lay = block->getLayer();

    srv->getWorld()->setBlock(pos.x, pos.y, lay, block);

    logD("Player changed block {}, {}, {}", pos.x, pos.y, lay);
    srv->broadcast(packet);
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
    auto pos = packet.get<Chunk::Position>();
    auto chunk = world->getChunk(pos);

    if (chunk == nullptr) {
        chunk = world->getGenerator()->generateChunk(pos);
        world->addChunk(chunk);
    }

    sendObj(chunk);
}