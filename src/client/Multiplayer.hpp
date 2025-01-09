#pragma once
#ifdef _WIN32
    #define NOGDI
    #define NOUSER
#endif

#include <PacketManager.hpp>
#include <entity/Player.hpp>
#include <world/Chunk.hpp>
#include <GamePacket.hpp>
#include <Types.hpp>
#include <memory>
#include <map>

class Multiplayer : private PacketManager {
private:
    bool m_connected = false;
    PlayerID m_myPlayerId;
    std::vector<Chunk::Position> m_chunkRequests;
    ENetHost* m_client;

public:
    using PacketManager::addToQueue;

    static auto get() {
        static auto inst = std::make_shared<Multiplayer>();
        return inst;
    }

    Multiplayer();
    ~Multiplayer();

    bool connect(std::string const& host, uint16_t port);
    bool connected();

    void onBlockChanged(Vec2i pos, uint8_t layer);
    void requestChunk(Chunk::Position pos);

    void update();
    void outThread();

    void handle(Packet& packet) override;
    void handleError(Packet& packet);
    void handleChunk(Packet& packet);
    // void handleBlock(Packet& packet);
    void handlePlayer(Packet& packet);
    void handleLoadPlayer(Packet& packet);
    void handleUnloadPlayer(Packet& packet);
    void handleBlockPlace(Packet& packet);
    void handleBlockDestroy(Packet& packet);

    std::string const getAddress();
    uint16_t const getPort();
};