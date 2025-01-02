#pragma once

#ifdef _WIN32
    #define NOGDI
    #define NOUSER
#endif

#include <sockpp/tcp_connector.h>
#include <GamePacket.hpp>
#include <PacketManager.hpp>
#include <Types.hpp>
#include <memory>
#include <map>

class Multiplayer : private PacketManager<sockpp::tcp_connector> {
private:
    bool m_connected = false;

public:
    using PacketManager::addToQueue;

    static auto get() {
        static auto inst = std::make_shared<Multiplayer>();
        return inst;
    }

    Multiplayer();

    bool connect(std::string const& host, in_port_t port);
    bool connected();

    void onBlockChanged(Vec2i pos, uint8_t layer);

    void inThread();
    void outThread();

    void handle(GamePacket& packet) override;
    void handleError(GamePacket& packet);
    void handleChunk(GamePacket& packet);
    void handleBlock(GamePacket& packet);
    void handlePlayer(GamePacket& packet);
    void handleLoadPlayer(GamePacket& packet);
    void handleUnloadPlayer(GamePacket& packet);
};