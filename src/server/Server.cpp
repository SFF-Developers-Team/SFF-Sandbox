#include <Server.hpp>
#include <Logger.hpp>
#include <World.hpp>
#include <WorldGenNormal.hpp>
#include <GamePacket.hpp>
#include <defaultconfig.hpp>
#include <Chunk.hpp>
#include <filesystem>
#include <fstream>

#if defined(_WIN32)
  #define WOULDBLOCK WSAEWOULDBLOCK
#elif defined(__linux__)
  #define WOULDBLOCK EWOULDBLOCK
#else
  #error Unknown platform WOULDBLOCK error code
#endif

using namespace std::chrono_literals;

void Server::init() {
#ifdef _WIN32
    setlocale(LOCALE_ALL, "ru");
#endif
    logD("SFF Sandbox server is starting...");

    if(!std::filesystem::exists("config.toml")) {
        std::ofstream file("config.toml");
        if(!file.is_open()) {
            logE("Can't create config file!");
            std::exit(1);
        }

        file.write(defaultConfig.data(), defaultConfig.size());
    }

    config = toml::parse_file("config.toml");
    
    sockpp::initialize();

    std::error_code ec;
    m_acceptor = sockpp::tcp_acceptor(config["port"].value_or(7777));
    m_acceptor.set_non_blocking();

    if(ec) {
        logE("Error creating the acceptor: {}", ec.message());
        exit(1);
    }

    m_pacman = new PacketManager(MP_BUF_SIZE);

    logD("Server listening *:{}", m_acceptor.address().port());

    m_world = new World(256, 128);
    m_timer = new Timer(60);

    if(!m_world->load()) {
        logD("Generating world...");
        m_world->generate(new WorldGenNormal(m_world));
    }

    std::thread inpthr(&Server::inputThread, this);
    inpthr.detach();

    loop();
}

void Server::inputThread() {
    std::string command;
    while(std::cin >> command) {
        std::string arg;
        std::stringstream ss(command);
        std::vector<std::string> args;
        while(getline(ss, arg, ' ')) args.push_back(arg);


        if(args[0] == "stop") destroy();
    }
}

void Server::loop() {
    while(true) {
        m_timer->advanceTime();

        for(uint32_t i = 0; i < m_timer->getTicks(); i++) {
            onTick();
        }
    }
}

void Server::destroy() {
    logD("Saving world...");
    Server::get()->getWorld()->save();
    
    delete m_pacman;
    std::exit(0);
}

void Server::onTick() {
    sockpp::inet_address peer;
    auto res = m_acceptor.accept(&peer);

    if(res) {
        sockpp::tcp_socket sock = res.release();
        std::thread thread(&Server::sessionThread, this, std::move(sock));
        thread.detach();
    }
}

void Server::sessionThread(sockpp::tcp_socket sock) {
    ByteVector buf(MP_BUF_SIZE);
    PlayerID playerId = 0;

    auto iden = m_pacman->recv(sock);
    if(iden->getBytes<SerializedObject::Header>() != SerializedObject::Header::IDENTIFICATION) {
        m_pacman->send(sock, CREATE_PACKET(SerializedObject::Header::NETWORK_ERROR, "First packet should be identification!"));
        return;
    }

    auto username = iden->getBytes<std::string>();

    if(m_world->isUsernameAlreadyTaken(username)) {
        m_pacman->send(sock, CREATE_PACKET(SerializedObject::Header::NETWORK_ERROR, "Username already taken!"));
        return;
    }

    playerId = joinPlayer(username);
    m_pacman->send(sock, CREATE_PACKET(SerializedObject::Header::IDENTIFICATION, playerId));

    m_clientQueue.insert(std::make_pair(playerId, std::vector<std::shared_ptr<GamePacket>>()));
    sock.set_non_blocking();

    auto& queue = m_clientQueue[playerId];

    // for(auto& [id, player] : m_world->getPlayers()) {
    //     if(id == playerId) continue;

    //     auto packet = CREATE_PACKET(SerializedObject::LOAD_PLAYER, id);
    //     packet->addBytes(player->getUsername());
    //     addToQueue(playerId, packet);
    // }

    // logD("Sent all players to player");

    // for(auto& chunk : m_world->getChunks()) {
    //     addToQueue(playerId, CREATE_PACKET(chunk->serialize()));
    // }

    // logD("Sent all chunks to player (queue size: {})", queue.size());

    bool canSendNext = true;

    while(true) {
        for(uint32_t i = 0; i < m_timer->getTicks(); i++) {
            if(queue.size() > 0 && canSendNext) {
                // logD("{} Queue size: {}", playerId, queue.size());
                if(m_pacman->send(sock, *queue.begin())) { 
                    queue.erase(queue.begin());
                }
            }
            
            auto packet = m_pacman->recv(sock);
            if (!packet) {
                canSendNext = false;
                continue;
            }

            switch(packet->getBytes<SerializedObject::Header>()) {
                case SerializedObject::Header::PLAYER: {
                    m_world->getPlayer(playerId)->deserialize(packet->serialize());
                    if(queue.empty()) {
                        auto players = CREATE_PACKET(SerializedObject::PLAYERS, m_world->getPlayers().size() - 1);

                        for(auto [id, player] : m_world->getPlayers()) {
                            player->setID(id);
                            if(id == playerId) continue;
                            players->addBytes(player->serialize());
                        }
                        
                        addToQueue(playerId, players);
                    }
                    break;
                }
                
                case SerializedObject::Header::BLOCK: {
                    auto block = std::make_unique<Block>(Block::BlockType::AIR);
                    block->deserialize(packet->serialize());

                    auto pos = block->getPosition();
                    auto layer = block->getLayer();

                    // logD("Block set {} {} {}", pos.x, pos.y, layer);

                    addToQueueAll(CREATE_PACKET(block->serialize()));
                    m_world->setBlock(pos.x, pos.y, layer, std::move(block));
                    break;
                }

                case SerializedObject::Header::LOAD_CHUNK: {
                    auto position = packet->getBytes<ChunkPosition>();
                    auto chunk = m_world->getChunk(position);

                    if(chunk) {
                        addToQueue(playerId, CREATE_PACKET(chunk->serialize()));
                        logD("Sent chunk {} to player {} ({} bytes)", chunk->getPosition(), playerId, chunk->serialize().size());
                    }
                    
                    break;
                }

                case SerializedObject::Header::LOAD_PLAYER: {
                    auto id = packet->getBytes<PlayerID>();
                    // logD("LOAD PLAYER {}", id);
                    if(!m_world->getPlayer(id)) {
                        addToQueue(playerId, CREATE_PACKET(SerializedObject::LOAD_PLAYER, 0));
                        break;
                    }

                    auto playerInfo = CREATE_PACKET(SerializedObject::LOAD_PLAYER, id);
                    playerInfo->addBytes(m_world->getPlayer(id)->getUsername());
                    addToQueue(playerId, playerInfo);
                    break;
                }

                case SerializedObject::Header::NETWORK_ERROR:
                case SerializedObject::Header::DISCONNECT: {
                    disconnectPlayer(playerId);
                    return;
                }
            }

            canSendNext = true;
        }
    }
}

void Server::addToQueueAll(std::shared_ptr<GamePacket> packet) {
    for(auto& [_, client] : m_clientQueue) {
        client.push_back(packet);
    }
}

void Server::addToQueue(PlayerID id, std::shared_ptr<GamePacket> packet) {
    m_clientQueue[id].push_back(packet);
}

void Server::addToQueueExcept(PlayerID id, std::shared_ptr<GamePacket> packet) {
    for(auto& [playerID, client] : m_clientQueue) {
        if(playerID != id) client.push_back(packet);
    }
}

PlayerID Server::joinPlayer(std::string const& username) {
    auto playerId = m_world->addPlayer(new SimplePlayer(m_world));
    m_world->getPlayer(playerId)->setUsername(username);
    logD("{} ({}) joined the game", username, playerId);

    auto packet = CREATE_PACKET(SerializedObject::Header::LOAD_PLAYER, playerId);
    packet->addBytes(username);

    addToQueueExcept(playerId, packet);

    return playerId;
}

void Server::disconnectPlayer(PlayerID id) {
    logD("{} ({}) left the game", m_world->getPlayer(id)->getUsername(), id);
    addToQueueExcept(id, CREATE_PACKET(SerializedObject::Header::UNLOAD_PLAYER, id));
    m_world->unloadPlayer(id);
    m_clientQueue.erase(id);
}