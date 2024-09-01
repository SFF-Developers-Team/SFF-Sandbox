#include <Server.hpp>
#include <Logger.hpp>
#include <World.hpp>
#include <WorldGenNormal.hpp>
#include <GamePacket.hpp>
#include <defaultconfig.hpp>
#include <Chunk.hpp>
#include <Timer.hpp>
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

    logD("Server listening *:{}", m_acceptor.address().port());

    m_world = new World(256, 128, "world");
    m_timer = new Timer(60);

    if(!m_world->load()) {
        logD("Generating world...");
        m_world->setGenerator(std::make_shared<WorldGenNormal>(m_world, 1));
        m_world->generate();
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

    std::exit(0);
}

void Server::onTick() {
    sockpp::inet_address peer;
    auto res = m_acceptor.accept(&peer);

    if(res) {
        sockpp::tcp_socket sock = res.release();
        std::thread thread(&Server::acceptThread, this, std::move(sock));
        thread.detach();
    }

    for(auto& client : m_clients) {
        client->onTick();
    }
}

void Server::acceptThread(sockpp::tcp_socket sock) {
    auto client = std::make_unique<Client>(std::move(sock));
    if(client->accept()) {
        m_clients.push_back(std::move(client));
        return;
    }

    client.release();
}

void Server::addToQueueAll(std::shared_ptr<GamePacket> packet) {
    for(auto& client : m_clients) {
        client->addToQueue(packet);
    }
}

void Server::addToQueue(PlayerID id, std::shared_ptr<GamePacket> packet) {
    for(auto& client : m_clients) {
        if(client->getPlayerID() == id) {
            client->addToQueue(packet);
            return;
        }
    }
}

void Server::addToQueueExcept(PlayerID id, std::shared_ptr<GamePacket> packet) {
    for(auto& client : m_clients) {
        if(client->getPlayerID() != id) client->addToQueue(packet);
    }
}

void Server::notifyAll(PlayerID id) {
    for(auto& client : m_clients) {
        if(client->getPlayerID() != id) client->notify(id);
    }
}

PlayerID Server::joinPlayer(std::string const& username) {
    auto playerId = m_world->addPlayer(std::make_unique<SimplePlayer>(m_world));
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

    for(auto i = m_clients.begin(); i != m_clients.end(); i++) {
        if((*i)->getPlayerID() == id) {
            (*i).release();
            m_clients.erase(i);
            return;
        }
    }
}