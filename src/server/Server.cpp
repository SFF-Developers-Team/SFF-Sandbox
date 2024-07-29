#include <Server.hpp>
#include <Logger.hpp>
#include <World.hpp>
#include <WorldGenNormal.hpp>
#include <GamePacket.hpp>
#include <Chunk.hpp>
#include <mutex>

#define MAX_CLIENTS 32
#define HOST "localhost"
#define PORT 7777

using namespace std::chrono_literals;

void Server::init() {
    setlocale(LOCALE_ALL, "ru");
    logD("SFF Sandbox server is starting...");

    sockpp::initialize();

    std::error_code ec;
    m_acceptor = sockpp::tcp_acceptor(PORT);

    if(ec) {
        logE("Error creating the acceptor: {}", ec.message());
        exit(1);
    }

    logD("Server listening *:{}", PORT);

    m_world = new World(256, 128);

    if(!m_world->load()) {
        logD("Generating world...");
        m_world->generate(new WorldGenNormal(m_world));
    }

    loop();
    destroy();
}

void Server::loop() {
    while(true) {
        sockpp::inet_address peer;
        auto res = m_acceptor.accept(&peer);

        if(!res) {
            logE("Failed to connect player {}", res.error_message());
            continue;
        }
        
        sockpp::tcp_socket sock = res.release();
        std::thread thread(&Server::acceptThread, this, std::move(sock));
        thread.detach();
    }
}

void Server::destroy() {
    
    exit(0);
}

void Server::acceptThread(sockpp::tcp_socket sock) {
    ByteVector buf(MP_BUF_SIZE);
    PlayerID playerId = -1;

    auto iden = read(sock, 512);
    if(iden->getBytes<SerializedObject::Header>() != SerializedObject::Header::IDENTIFICATION) {
        send(sock, new GamePacket(SerializedObject::Header::SERVER_ERROR, "First packet should be identification!"));
        return;
    }

    auto username = iden->getBytes<std::string>();

    if(m_world->isUsernameAlreadyTaken(username)) {
        send(sock, new GamePacket(SerializedObject::Header::SERVER_ERROR, "Username already taken!"));
        return;
    }

    playerId = joinPlayer(username);
    send(sock, new GamePacket(SerializedObject::Header::IDENTIFICATION, playerId));

    m_clientQueue.insert(std::make_pair(playerId, std::vector<GamePacket*>()));
    sock.set_non_blocking();

    auto& queue = m_clientQueue[playerId];

    for(auto& chunk : m_world->getChunks()) {
        addToQueue(playerId, new GamePacket(chunk->serialize()));
    }

    logD("Sent all chunks to player");

    for(auto& [id, player] : m_world->getPlayers()) {
        if(id == playerId) continue;

        auto packet = new GamePacket(SerializedObject::LOAD_PLAYER, id);
        packet->addBytes(player->getUsername());
        addToQueue(playerId, packet);

        auto packetPos = new GamePacket(SerializedObject::PLAYER_POSITION, id);
        packetPos->addBytes(player->getPosition());
        addToQueue(playerId, packetPos);

        auto packetAnim = new GamePacket(SerializedObject::PLAYER_ANIMATION, id);
        packetAnim->addBytes(player->getAnimCurrentFrame());
        addToQueue(playerId, packetAnim);

        auto packetDir = new GamePacket(SerializedObject::PLAYER_DIRECTION, id);
        packetDir->addBytes(player->getDirection());
        addToQueue(playerId, packetDir);
    }

    logD("Sent all players to player");

    while (true) {
        auto packet = read(sock, buf);
        if (!packet) break;

        switch(packet->getBytes<SerializedObject::Header>()) {
            case SerializedObject::Header::PLAYER_POSITION: {
                auto pos = packet->getBytes<Vec2f>();
                m_world->getPlayer(playerId)->setPosition(pos);
                
                auto packet = new GamePacket(SerializedObject::PLAYER_POSITION, playerId);
                packet->addBytes(pos);

                addToQueueExcept(playerId, packet);
                break;
            }

            case SerializedObject::Header::PLAYER_ANIMATION: {
                auto frame = packet->getBytes<uint8_t>();
                m_world->getPlayer(playerId)->setAnimCurrentFrame(frame);
                
                auto packet = new GamePacket(SerializedObject::PLAYER_ANIMATION, playerId);
                packet->addBytes(frame);

                addToQueueExcept(playerId, packet);
                break;
            }

            case SerializedObject::Header::PLAYER_DIRECTION: {
                auto dir = packet->getBytes<SimplePlayer::Direction>();
                m_world->getPlayer(playerId)->turn(dir);

                auto packet = new GamePacket(SerializedObject::PLAYER_DIRECTION, playerId);
                packet->addBytes(dir);

                addToQueueExcept(playerId, packet);
                break;
            }
        }

        delete packet;

        if(queue.size() > 0) {
            send(sock, *queue.begin()); 
            queue.erase(queue.begin());
        }
    }

    disconnectPlayer(playerId);
}

void Server::addToQueueAll(GamePacket* packet) {
    for(auto& [_, client] : m_clientQueue) {
        client.push_back(packet);
    }
}

void Server::addToQueue(PlayerID id, GamePacket* packet) {
    m_clientQueue[id].push_back(packet);
}

void Server::addToQueueExcept(PlayerID id, GamePacket* packet) {
    for(auto& [playerID, client] : m_clientQueue) {
        if(playerID != id) client.push_back(packet);
    }
}

GamePacket* Server::read(sockpp::tcp_socket& sock, ByteVector& buf) {
    auto result = sock.read(buf.data(), buf.size());
    
    // Ееее рок хардкод (10035 - EWOULDLOCK)
    if(result.is_error() && result.error().value() == 10035) {
        return new GamePacket(SerializedObject::Header::NULL_PACKET); 
    }

    return (result.is_ok() ? new GamePacket(buf) : nullptr);
}

GamePacket* Server::read(sockpp::tcp_socket& sock, size_t n) {
    auto buf = ByteVector(n);
    return read(sock, buf);
}

GamePacket* Server::read(sockpp::tcp_socket& sock) {
    return read(sock, MP_BUF_SIZE);
}

bool Server::send(sockpp::tcp_socket& sock, GamePacket* packet) {
    auto bytes = packet->serialize();
    sockpp::result<socket_t> result = sock.write(bytes.data(), bytes.size());

    delete packet;
    return result.is_ok();
}

PlayerID Server::joinPlayer(std::string const& username) {
    auto playerId = m_world->addPlayer(new SimplePlayer(m_world));
    m_world->getPlayer(playerId)->setUsername(username);
    logD("{} ({}) joined the game", username, playerId);

    auto packet = new GamePacket(SerializedObject::Header::LOAD_PLAYER, playerId);
    packet->addBytes(username);

    addToQueueExcept(playerId, packet);

    return playerId;
}

void Server::disconnectPlayer(PlayerID id) {
    logD("{} ({}) left the game", m_world->getPlayer(id)->getUsername(), id);
    addToQueueExcept(id, new GamePacket(SerializedObject::Header::UNLOAD_PLAYER, id));
    m_world->unloadPlayer(id);
    m_clientQueue.erase(id);
}