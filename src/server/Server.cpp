#include <world/gen/WorldGenNormal.hpp>
#include <DefaultConfig.hpp>
#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <Packet.hpp>
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
        .host = ENET_HOST_ANY,
        .port = config["port"].value_or<uint16_t>(7777)
    };

    if(cfgaddr != "*") {
        enet_address_set_host(&address, cfgaddr.data());
    }

    auto maxclients = config["max-players"].value_or(32);
    auto inbandwidth = config["incoming-bandwidth"].value_or(0);
    auto outbandwidth = config["outcoming-bandwidth"].value_or(0);
    m_server = enet_host_create(&address, maxclients, 4, inbandwidth, outbandwidth);

    if(m_server == NULL) {
        logE("An error occurred while trying to create an ENet server host.");
        std::exit(1);
    }

    logD("Server listening {}:{}", cfgaddr, address.port);

    auto worldName = config["world-name"].value_or<std::string>("world");

    m_world = std::make_shared<World>(128, worldName);
    m_timer = std::make_shared<Timer>(60);

    if(!m_world->load()) {
        logD("Generating world...");
        srand(time(NULL));
        auto seed = config["world-seed"].value_or(rand());

        std::shared_ptr<WorldGen> generator;
        switch(config["world-generator"].value_or(0)) {
            default:
            case 0: generator = std::make_shared<WorldGenNormal>(m_world, seed); break;
            // Oh fu*k I thought we have more worldgens
        }

        m_world->setGenerator(generator);
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
    m_timer->advanceTime();

    for(int i = 0; i < m_timer->getTicks(); i++) {
        this->onTick();
        m_world->onTick();
    }
}

void Server::onTick() {
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
                    m_clients[id]->packetReceived(pak);
                    enet_packet_destroy(event.packet);
                }

            default:
                break;
        }
    }
}

void Server::destroy() {
    logD("Saving world...");
    Server::get()->getWorld()->save();

    std::exit(0);
}

void Server::broadcast(std::shared_ptr<SerializedObject> obj, Channel channel, bool reliable) {
    uint32_t flag = (reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
    auto bytes = obj->serialize();
    auto packet = enet_packet_create(bytes.data(), bytes.size(), flag);
    enet_host_broadcast(m_server, static_cast<uint8_t>(channel), packet);
}

void Server::broadcast(Packet const& packet, Channel channel, bool reliable) {
    uint32_t flag = (reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
    auto enetPacket = enet_packet_create(packet.data(), packet.size(), flag);
    enet_host_broadcast(m_server, static_cast<uint8_t>(channel), enetPacket);
}

void Server::broadcastExcept(PlayerID pid, std::shared_ptr<SerializedObject> obj, Channel channel, bool reliable) {
    for(auto& [_, client] : m_clients) {
        if(client->getPlayerID() != pid) {
            client->sendObj(obj, channel, reliable);
        }
    }
}

void Server::broadcastExcept(PlayerID pid, Packet const& packet, Channel channel, bool reliable) {
    for(auto& [_, client] : m_clients) {
        if(client->getPlayerID() != pid) {
            client->sendPacket(packet, channel, reliable);
        }
    }
}

void Server::send(PlayerID pid, std::shared_ptr<SerializedObject> obj, Channel channel, bool reliable) {
    for(auto& [_, client] : m_clients) {
        if(client->getPlayerID() == pid) {
            client->sendObj(obj, channel, reliable);
            break;
        }
    }
}

void Server::send(PlayerID pid, Packet const& packet, Channel channel, bool reliable) {
    for(auto& [_, client] : m_clients) {
        if(client->getPlayerID() == pid) {
            client->sendPacket(packet, channel, reliable);
            break;
        }
    }
}

PlayerID Server::joinPlayer(std::string const& username) {
    auto playerId = m_world->addPlayer(std::make_unique<SimplePlayer>(m_world), username);
    logD("{} joined the game", username);

    auto packet = Packet(Header::LOAD_PLAYER);
    packet.add(playerId);
    packet.add(username);

    broadcast(packet, NOTIFICATIONS, true);

    return playerId;
}

void Server::disconnectPlayer(std::shared_ptr<Client> client, DisconnectReasonID reason) {    
    auto id = client->getPlayerID();
    logD("{} left the game", m_world->getPlayer(id)->getUsername());
    
    m_world->unloadPlayer(id);

    std::erase_if(m_clients, [&](auto& pair) { return pair.second->getPlayerID() == id; });
    broadcast(Packet(Header::UNLOAD_PLAYER, id), NOTIFICATIONS);
}

std::string const Server::getDisconnectReasonByID(DisconnectReasonID id) {
    switch(id) {
        case INVALID_FIRST_PACKET: return "First packet should be identification!";
        case TOO_SHORT_USERNAME: return "Too short username!";
        case TOO_LONG_USERNAME: return "Too long username!";
        case USERNAME_ALREADY_TAKEN: return "Username already taken!";
        default: return "What?!";
    }
}