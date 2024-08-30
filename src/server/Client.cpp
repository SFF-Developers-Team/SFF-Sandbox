#include <Client.hpp>
#include <Server.hpp>
#include <World.hpp>
#include <Chunk.hpp>
#include <Logger.hpp>
#include <mutex>

using Header = SerializedObject::Header;

Client::Client(sockpp::tcp_socket sock) : m_sock(std::move(sock)) {
    m_pacman = std::make_unique<PacketManager<sockpp::tcp_socket>>(m_sock, MP_BUF_SIZE);
    m_sock.set_non_blocking();
}

Client::~Client() {
    m_pacman.release();
}

void Client::addToQueue(std::shared_ptr<GamePacket> packet) {
    m_queue.push_back(packet);

    if(m_queue.size() > 10) logW("big queue player {} size {}", m_id, m_queue.size());
}

void Client::notify(PlayerID id) {
    if(std::find(m_notifications.begin(), m_notifications.end(), id) != m_notifications.end()) return;
    m_notifications.push_back(id);
}

bool Client::accept() {
    auto srv = Server::get();
    const std::lock_guard<std::mutex> lock(srv->getAcceptLock());

    std::shared_ptr<GamePacket> identification = nullptr;
    while(!identification) identification = m_pacman->recv();
    if(identification->getBytes<Header>() != Header::IDENTIFICATION) {
        while(!m_pacman->send(CREATE_PACKET(Header::NETWORK_ERROR, "First packet should be identification!")));
        return false;
    }

    auto username = identification->getBytes<std::string>();

    if(username.length() > 16) {
        while(!m_pacman->send(CREATE_PACKET(SerializedObject::Header::NETWORK_ERROR, "Too long username!")));
        return false;
    }

    if(srv->getWorld()->isUsernameAlreadyTaken(username)) {
        while(!m_pacman->send(CREATE_PACKET(SerializedObject::Header::NETWORK_ERROR, "Username already taken!")));
        return false;
    }

    m_id = srv->joinPlayer(username);
    while(!m_pacman->send(CREATE_PACKET(SerializedObject::Header::IDENTIFICATION, m_id)));

    uint8_t chunksCount = 16;
    auto worldPacket = CREATE_PACKET(Header::CHUNKS, chunksCount);

    while(chunksCount-- > 0) {
        auto bytes = srv->getWorld()->getChunk(chunksCount)->serialize();
        worldPacket->addBytes((uint16_t)bytes.size());
        worldPacket->addBytes(bytes);
    }

    logD("Sending world to player {}...", username);
    while(!m_pacman->send(worldPacket));

    // for(auto& [id, player] : srv->getWorld()->getPlayers()) {
    //     if(player->getID() == id) continue;
    //     addToQueue(CREATE_PACKET(player->serialize()));
    // }

    return true;
}

void Client::onTick() {
    auto srv = Server::get();

    if(m_queue.empty()) {
        for(auto id : m_notifications) {
            auto player = srv->getWorld()->getPlayer(id);
            if(!player) continue;

            addToQueue(CREATE_PACKET(player->serialize()));
        }

        m_notifications.clear();
    }

    if(!m_queue.empty() && m_pacman->send(*m_queue.begin())) {
        m_queue.erase(m_queue.begin());
        return;
    }

    auto packet = m_pacman->recv();
    if (!packet) return;

    switch(packet->getBytes<SerializedObject::Header>()) {
        case SerializedObject::Header::PLAYER: {
            srv->getWorld()->getPlayer(m_id)->deserialize(packet->serialize());
            srv->notifyAll(m_id);

            break;
        }
        
        case SerializedObject::Header::BLOCK: {
            auto block = std::make_unique<Block>(Block::BlockType::AIR);
            block->deserialize(packet->serialize());

            auto pos = block->getPosition();
            auto layer = block->getLayer();

            logD("Set block [{}, {}, {}]", pos.x, pos.y, layer);

            srv->addToQueueExcept(m_id, CREATE_PACKET(block->serialize()));
            srv->getWorld()->setBlock(pos.x, pos.y, layer, std::move(block));

            break;
        }

        case SerializedObject::Header::LOAD_CHUNK: {
            auto position = packet->getBytes<ChunkPosition>();
            auto chunk = srv->getWorld()->getChunk(position);

            if(chunk) {
                addToQueue(CREATE_PACKET(chunk->serialize()));
                logD("Sent chunk {} to player {}", chunk->getPosition(), m_id);
            }
            
            break;
        }

        case SerializedObject::Header::LOAD_PLAYER: {
            auto id = packet->getBytes<PlayerID>();
            logD("LOAD PLAYER {}", id);
            if(!srv->getWorld()->getPlayer(id)) {
                addToQueue(CREATE_PACKET(SerializedObject::UNLOAD_PLAYER, id));
                break;
            }

            auto playerInfo = CREATE_PACKET(SerializedObject::LOAD_PLAYER, id);
            playerInfo->addBytes(srv->getWorld()->getPlayer(id)->getUsername());
            addToQueue(playerInfo);

            break;
        }

        case SerializedObject::Header::NETWORK_ERROR:
        case SerializedObject::Header::DISCONNECT: {
            srv->disconnectPlayer(m_id);
            return;
        }
    }
}