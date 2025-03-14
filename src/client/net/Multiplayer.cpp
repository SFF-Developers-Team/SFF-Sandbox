#include <entity/OnlinePlayer.hpp>
#include <entity/Player.hpp>
#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <net/Multiplayer.hpp>
#include <Logger.hpp>
#include <managers/Debug.hpp>
#include <Game.hpp>
#include <thread>

Multiplayer::Multiplayer() : PacketManager(nullptr), m_client(nullptr) {
    if (enet_initialize() != 0) {
        logE("An error occurred while initializing ENet.");
    }
}

Multiplayer::~Multiplayer() {
    destroy();
    enet_deinitialize();
}

void Multiplayer::error(std::string const& str) {
    m_connected = false;
    m_state = ERROR;
    m_error = str;
}

bool Multiplayer::connect(std::string const& host, uint16_t port) {
    ENetAddress address;
    ENetEvent event;

    logD("Connecting to {}:{}...", host, port);

    m_state = CONNECTING;
    m_client = enet_host_create(0, 1, Channel::LAST_CHANNEL, 0, 0);

    if(!m_client) {
        error("Failed to create client!");
        return false;
    }

    enet_address_set_host(&address, host.c_str());
    address.port = port;

    m_peer = enet_host_connect(m_client, &address, Channel::LAST_CHANNEL, 0);

    auto res = enet_host_service(m_client, &event, 5000);

    if(m_peer == nullptr || m_client == nullptr) {
        return false;
    }

    if (res < 0 || event.type != ENET_EVENT_TYPE_CONNECT) {
        enet_peer_reset(m_peer);
        error("Failed to connect to server!");
        return false;
    }

    auto game = Game::get();
    std::string myUsername = game->getUsername();

    auto packet = Packet(ObjectHeader::IDENTIFICATION);
    packet.add(myUsername);
    
    if(sendPacket(packet)) {
        logD("Identification sent. Waiting for response...");
        m_state = LOGGING_IN;
    }

    return true;
}

void Multiplayer::update() {
    ENetEvent event;
    while(enet_host_service(m_client, &event, 0) > 0) {
        switch(event.type) {
            case ENET_EVENT_TYPE_RECEIVE: {
                auto packet = Packet(event.packet);
                enet_packet_destroy(event.packet);

                if(!m_connected) {
                    auto header = packet.get<ObjectHeader>();
                    if(header == ObjectHeader::NETWORK_ERROR) handleError(packet);
                    if(header != ObjectHeader::IDENTIFICATION) break;

                    m_myPlayerId = packet.get<PlayerID>(0);
                    logD("Received PlayerID from server {}", m_myPlayerId);
                    
                    Game::get()->setWorld(std::make_shared<World>("mp"));

                    m_connected = true;
                    m_state = LOADING_TERRAIN;

                    sendPacket(Packet(ObjectHeader::LOAD_TERRAIN));
                    sendPacket(Packet(ObjectHeader::LOAD_MESSAGE));

                    logD("Sending load message");
                    break;
                }

                handle(packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                error("Timed out.");
                break;

            case ENET_EVENT_TYPE_DISCONNECT: {
                error("Connection closed.");
                break;
            }

            default: break;
        }
    }
}

void Multiplayer::destroy() {
    if (m_peer) {
        enet_peer_reset(m_peer);
        m_peer = nullptr;
    }

    if(m_client) {
        enet_host_destroy(m_client);
        m_client = nullptr;
    }
}

void Multiplayer::handle(Packet& packet) {
    PacketManager::handle(packet);

    switch(packet.get<ObjectHeader>()) {
        case ObjectHeader::UNLOAD_PLAYER: return handleUnloadPlayer(packet);
        case ObjectHeader::LOAD_PLAYER: return handleLoadPlayer(packet);
        case ObjectHeader::NETWORK_ERROR: return handleError(packet);
        case ObjectHeader::PLAYER: return handlePlayer(packet);
        case ObjectHeader::CHUNK: return handleChunk(packet);
        case ObjectHeader::BLOCK_DESTROY: return handleBlockDestroy(packet);
        case ObjectHeader::BLOCK_PLACE: return handleBlockPlace(packet);
        case ObjectHeader::TERRAIN: return handleTerrain(packet);
        case ObjectHeader::MESSAGE: return handleMessage(packet);
        case ObjectHeader::LOAD_MESSAGE: return handleLoadMessage(packet);
        default: return;
    }
}

void Multiplayer::handleError(Packet& packet) {
    error(packet.get("Unknown error has occured"));
}

void Multiplayer::handleLoadMessage(Packet& packet) {
    auto message = packet.get<std::vector<std::string>>();

    for(int i = 0; i < message.size(); i++) {
        messages[i] = message[i];
    }
}
void Multiplayer::handleMessage(Packet& packet) {
    std::string msg = packet.get("");
    logD("{}", msg);

    messages.push_back(msg);
}
void Multiplayer::handlePlayer(Packet& packet) {
    auto game = Game::get();
    auto world = game->getWorld();
    auto me = game->getPlayer();
    auto id = packet.get<PlayerID>(0);
    
    if(id > 0 && me != nullptr) {
        if(id == me->getPlayerID()) {
            me->deserialize(packet);
            return;
        }

        auto otherPlayer = world->getPlayer(id);

        if(!otherPlayer) {
            otherPlayer = std::make_shared<OnlinePlayer>(world);
            world->setPlayer(id, otherPlayer);
        }

        if(otherPlayer->getUsername().empty()) {
            sendPacket(Packet(ObjectHeader::LOAD_PLAYER, id));
        }

        otherPlayer->deserialize(packet);
    }
}

void Multiplayer::handleLoadPlayer(Packet& packet) {
    auto game = Game::get();
    auto world = game->getWorld();
    auto id = packet.get<PlayerID>(0);

    logD("Load player {}", id);
    
    if(id > 0) {
        auto player = world->getPlayer(id);
        std::string username = packet.get("");
        
        if(!player) {
            player = std::make_shared<OnlinePlayer>(world);
            world->setPlayer(id, player);
        }

        player->setUsername(username);
    }
}

void Multiplayer::handleUnloadPlayer(Packet& packet) {
    auto game = Game::get();
    auto world = game->getWorld();
    auto id = packet.get<PlayerID>(0);
    logD("Unload player {}", id);

    if(world->getPlayer(id)) {
        world->unloadPlayer(id);
    }
}

void Multiplayer::handleChunk(Packet& packet) {
    auto game = Game::get();
    auto world = game->getWorld();
    auto pos = packet.get<Vec2i>();
    packet.reset();

    auto chunk = std::make_shared<Chunk>();
    chunk->deserialize(packet);
    world->addChunk(pos, chunk);
}

void Multiplayer::handleTerrain(Packet& packet) {
    if(packet.get<ObjectHeader>() == ObjectHeader::ARRAY) {
        handleArray(packet);
    }

    auto game = Game::get();
    auto world = game->getWorld();
    auto player = std::make_shared<Player>(world);
    game->setPlayer(player);
    world->setPlayer(m_myPlayerId, player);
    m_state = PLAYING;

    sendPacket(Packet(ObjectHeader::LOAD_PLAYERS));
}

void Multiplayer::handleBlockPlace(Packet& packet) {
    // auto game = Game::get();

    // auto blockBytes = DataStream();
    // blockBytes.deserialize(packet.getN(packet.size() - 1));
    // auto block = Block::create(blockBytes);
    // block->deserialize(blockBytes);

    // auto pos = block->getPos();
    // auto lay = block->getLayer();

    // game->getWorld()->setBlock(pos.x, pos.y, lay, block);
}

void Multiplayer::handleBlockDestroy(Packet& packet) {
    Game::get()->getWorld()->destroyBlock(packet.get<BlockPosition>());
}

std::string const Multiplayer::getAddress() {
    char buf[256];
    enet_address_get_host(&m_client->address, buf, sizeof(buf));
    return std::string(buf);
}