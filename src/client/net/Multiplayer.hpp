#pragma once
#ifdef _WIN32
    #define NOGDI
    #define NOUSER

    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    // Type required before windows.h inclusion
    typedef struct tagMSG *LPMSG;
#endif

#include <managers/PacketManager.hpp>
#include <entity/LocalPlayer.hpp>
#include <world/Chunk.hpp>
#include <Packet.hpp>
#include <Types.hpp>
#include <memory>
#include <map>

enum MultiplayerState {
    CONNECTING,
    LOGGING_IN,
    LOADING_TERRAIN,
    PLAYING,
    ERROR
};

class Multiplayer : public PacketManager {
private:
    bool m_connected = false;
    PlayerID m_myPlayerId;
    std::vector<std::string> messages;
    ENetHost* m_client;
    MultiplayerState m_state;
    std::string m_error;

public:
    static auto get() {
        static auto inst = std::make_shared<Multiplayer>();
        return inst;
    }

    Multiplayer();
    ~Multiplayer();

    bool connect(std::string const& host, uint16_t port);
    bool isConnected() { return m_connected; }
    
    void update();
    void error(std::string const& str);
    void destroy();

    void handle(Packet& packet) override;
    void handleError(Packet& packet);
    void handleChunk(Packet& packet);
    void handleTerrain(Packet& packet);
    void handlePlayer(Packet& packet);
    void handleLoadPlayer(Packet& packet);
    void handleUnloadPlayer(Packet& packet);
    void handleBlockPlace(Packet& packet);
    void handleBlockDestroy(Packet& packet);
    void handleMessage(Packet& packet);
    void handleLoadMessage(Packet& packet);
    std::string const& getError() { return m_error; }
    MultiplayerState const getState() { return m_state; }

    std::string const getAddress();
    uint16_t const getPort() { return m_client->address.port; }

    std::vector<std::string> getMessages() { return messages; }
};