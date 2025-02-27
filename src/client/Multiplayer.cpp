#include <entity/OnlinePlayer.hpp>
#include <entity/Player.hpp>
#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <Multiplayer.hpp>
#include <Logger.hpp>
#include <Debug.hpp>
#include <Game.hpp>
#include <thread>

using Header = SerializedObject::Header;

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

    auto packet = Packet(SerializedObject::Header::IDENTIFICATION);
    packet.add(myUsername);
    
    if(sendPacket(packet)) {
        logD("Identification sent. Waiting for response...");
        m_state = LOGGING_IN;
    }

    return true;
}

bool Multiplayer::connected() {
    return m_connected;
} 

void Multiplayer::requestChunk(Chunk::Position pos) {
    auto it = std::find(m_chunkRequests.begin(), m_chunkRequests.end(), pos);

    if(it == m_chunkRequests.end()) {
        m_chunkRequests.push_back(pos);
        sendPacket(Packet(Header::LOAD_CHUNK, pos));
    }
}

void Multiplayer::update() {
    ENetEvent event;
    while(enet_host_service(m_client, &event, 0) > 0) {
        switch(event.type) {
            case ENET_EVENT_TYPE_RECEIVE: {
                auto packet = Packet(event.packet);
                enet_packet_destroy(event.packet);

                if(!m_connected) {
                    auto header = packet.get<Header>();
                    if(header == Header::NETWORK_ERROR) handleError(packet);
                    if(header != Header::IDENTIFICATION) break;

                    m_myPlayerId = packet.get<PlayerID>(0);
                    logD("Received PlayerID from server {}", m_myPlayerId);
                    
                    Game::get()->setWorld(std::make_shared<World>("mp"));

                    m_connected = true;
                    m_state = LOADING_TERRAIN;

                    sendPacket(Packet(Header::LOAD_TERRAIN));
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

    switch(packet.get<Header>()) {
        case Header::UNLOAD_PLAYER: return handleUnloadPlayer(packet);
        case Header::LOAD_PLAYER: return handleLoadPlayer(packet);
        case Header::NETWORK_ERROR: return handleError(packet);
        case Header::PLAYER: return handlePlayer(packet);
        case Header::CHUNK: return handleChunk(packet);
        case Header::BLOCK_DESTROY: return handleBlockDestroy(packet);
        case Header::BLOCK_PLACE: return handleBlockPlace(packet);
        case Header::TERRAIN: return handleTerrain(packet);
        default: return;
    }
}

void Multiplayer::handleError(Packet& packet) {
    error(packet.get("Unknown error has occured"));
}

void Multiplayer::handlePlayer(Packet& packet) {
    auto game = Game::get();
    auto world = game->getWorld();
    auto me = game->getPlayer();
    auto id = packet.get<PlayerID>(0);
    
    if(id > 0 && me != nullptr) {
        if(id == me->getPlayerID()) {
            me->deserialize(packet.bytes());
            return;
        }

        auto otherPlayer = world->getPlayer(id);

        if(!otherPlayer) {
            otherPlayer = std::make_shared<OnlinePlayer>(world);
            world->addPlayer(id, otherPlayer);
        }

        if(otherPlayer->getUsername().empty()) {
            sendPacket(Packet(SerializedObject::LOAD_PLAYER, id));
        }

        otherPlayer->deserialize(packet.bytes());
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
            world->addPlayer(id, player);
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
    auto chunk = std::make_shared<Chunk>(world);
    chunk->deserialize(packet.bytes());

    auto it = std::find(m_chunkRequests.begin(), m_chunkRequests.end(), chunk->getPosition());
    if(it != m_chunkRequests.end()) {
        m_chunkRequests.erase(it);
    }

    world->addChunk(chunk);
}

void Multiplayer::handleTerrain(Packet& packet) {
    if(packet.get<Header>() == Header::ARRAY) {
        handleArray(packet);
    }

    auto game = Game::get();
    auto world = game->getWorld();
    auto player = std::make_shared<Player>(world);
    game->setPlayer(player);
    world->addPlayer(m_myPlayerId, player);
    m_state = PLAYING;

    sendPacket(Packet(Header::LOAD_PLAYERS));
}

void Multiplayer::handleBlockPlace(Packet& packet) {
    auto game = Game::get();

    auto block = std::make_shared<Block>();
    block->deserialize(packet.getN(packet.size() - 1));

    auto pos = block->getPos();
    auto lay = block->getLayer();

    game->getWorld()->setBlock(pos.x, pos.y, lay, block);
}

void Multiplayer::handleBlockDestroy(Packet& packet) {
    auto x = packet.get<int32_t>();
    auto y = packet.get<int32_t>();
    auto l = packet.get<uint8_t>();

    Game::get()->getWorld()->destroyBlock(x, y, l);
}

std::string const Multiplayer::getAddress() {
    char buf[256];
    enet_address_get_host(&m_client->address, buf, sizeof(buf));
    return std::string(buf);
}