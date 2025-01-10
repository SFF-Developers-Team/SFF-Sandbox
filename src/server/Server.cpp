#include <world/gen/WorldGenNormal.hpp>
#include <DefaultConfig.hpp>
#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <GamePacket.hpp>
#include <Server.hpp>
#include <Logger.hpp>
#include <Timer.hpp>

#include <string_view>
#include <filesystem>
#include <fstream>

using namespace std::string_view_literals;

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

    auto cfgaddr = config["address"].value_or("*"sv);

    ENetAddress address = {
        .port = config["port"].value_or<uint16_t>(7777)
    };

    if(cfgaddr == "*") {
        address.host = ENET_HOST_ANY;
    } else {
        enet_address_set_host(&address, cfgaddr.data());
    }

    m_server = enet_host_create(&address, 32, 2, 0, 0);

    if(m_server == NULL) {
        logE("An error occurred while trying to create an ENet server host.");
        std::exit(1);
    }

    logD("Server listening {}:{}", cfgaddr, address.port);

    m_world = std::make_shared<World>(128, "world");
    m_timer = std::make_shared<Timer>(60);

    if(!m_world->load()) {
        logD("Generating world...");
        m_world->setGenerator(std::make_shared<WorldGenNormal>(m_world, 1));
        m_world->generate();
    }

    std::thread inpthr(&Server::inputThread, this);
    inpthr.detach();

    while(true) {
        update();
    }
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

void Server::update() {
    ENetEvent event;

    while(enet_host_service(m_server, &event, 0) > 0) {
        auto id = event.peer->connectID;
        switch(event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                if(!m_clients.contains(id)) {
                    m_clients[id] = std::make_shared<Client>(event.peer);
                }

                break;

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
            case ENET_EVENT_TYPE_DISCONNECT: 
                if(m_clients.contains(id)) {
                    disconnectPlayer(m_clients[id], static_cast<DisconnectReasonID>(event.data));
                }

                break;

            case ENET_EVENT_TYPE_RECEIVE:
                if(m_clients.contains(id)) {
                    auto pak = Packet(event.packet);
                    m_clients[id]->onPacketReceived(pak);
                    enet_packet_destroy(event.packet);
                }

            default:
                break;
        }
    }

    for(auto& [_, client] : m_clients) {
        client->update();
    }
}

void Server::destroy() {
    logD("Saving world...");
    Server::get()->getWorld()->save();

    std::exit(0);
}

void Server::addToQueueAll(std::shared_ptr<SerializedObject> packet) {
    for(auto& [addr, client] : m_clients) {
        client->addToQueue(packet);
    }
}

void Server::addToQueue(PlayerID id, std::shared_ptr<SerializedObject> packet) {
    for(auto& [addr, client] : m_clients) {
        if(client->getPlayerID() == id) client->addToQueue(packet);
    }
}

void Server::addToQueueExcept(PlayerID id, std::shared_ptr<SerializedObject> packet) {
    for(auto& [addr, client] : m_clients) {
        if(client->getPlayerID() != id) client->addToQueue(packet);
    }
}

PlayerID Server::joinPlayer(std::string const& username) {
    auto playerId = m_world->addPlayer(std::make_unique<SimplePlayer>(m_world), username);
    logD("{} joined the game", username);

    auto packet = CREATE_PACKET(SerializedObject::Header::LOAD_PLAYER, playerId);
    packet->add(username);

    addToQueueExcept(playerId, packet);

    return playerId;
}

void Server::disconnectPlayer(std::shared_ptr<Client> client, DisconnectReasonID reason) {    
    auto id = client->getPlayerID();
    logD("{} left the game", m_world->getPlayer(id)->getUsername());
    
    m_world->unloadPlayer(id);

    std::erase_if(m_clients, [&](auto& pair) { return pair.second->getPlayerID() == id; });
    addToQueueAll(CREATE_PACKET(SerializedObject::Header::UNLOAD_PLAYER, id));
}

std::string const Server::getDisconnectReasonByID(DisconnectReasonID id) {
    switch(id) {
        case INVALID_FIRST_PACKET: return "First packet should be identification!";
        case TOO_SHORT_USERNAME: return "Too short username!";
        case TOO_LONG_USERNAME: return "Too long username!";
        case USERNAME_ALREADY_TAKEN: return "Username already taken!";
    }
}