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

    if (head != Header::IDENTIFICATION) {
        disconnect(INVALID_FIRST_PACKET);
        return false;
    }

    auto username = packet.get<std::string>();

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
    send(iden);

    uint16_t chunksCount = 3;

    while (chunksCount-- > 0) {
        addToQueue(srv->getWorld()->getChunk(chunksCount));
    }

    for (auto& [id, player] : srv->getWorld()->getPlayers()) {
        if (id == m_id)
            continue;
        addToQueue(CREATE_PACKET(Header::LOAD_PLAYER, id));
    }

    m_loggedIn = true;

    return true;
}

void Client::update() {
    if (m_loggedIn && !m_disconnect) {
        sendQueue();
    }
}

void Client::onPacketReceived(Packet& packet) {
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
    auto err = Packet(Header::NETWORK_ERROR, srv->getDisconnectReasonByID(reason));
    send(err);

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
    srv->addToQueueExcept(m_id, player);
}

void Client::handleLoadPlayer(Packet& packet) {
    auto srv = Server::get();
    auto id = packet.get<PlayerID>(0);
    auto player = srv->getWorld()->getPlayer(id);

    if (player != nullptr) {
        auto packet = CREATE_PACKET(Header::LOAD_PLAYER, id);
        packet->add(player->getUsername());
        return addToQueue(packet);
    }

    return addToQueue(CREATE_PACKET(Header::UNLOAD_PLAYER, id));
}

void Client::handleBlockPlace(Packet& packet) {
    auto srv = Server::get();

    auto block = std::make_shared<Block>();
    block->deserialize(packet.getN(packet.size() - 1));

    auto pos = block->getPos();
    auto lay = block->getLayer();

    srv->getWorld()->setBlock(pos.x, pos.y, lay, block);

    logD("Player changed block {}, {}, {}", pos.x, pos.y, lay);
    srv->addToQueueAll(CREATE_PACKET(packet.bytes()));
}

void Client::handleBlockDestroy(Packet& packet) {
    auto x = packet.get<int32_t>();
    auto y = packet.get<int32_t>();
    auto l = packet.get<uint8_t>();
    auto srv = Server::get();
    srv->getWorld()->destroyBlock(x, y, l);
    srv->addToQueueAll(CREATE_PACKET(packet.bytes()));
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

    return addToQueue(chunk);
}