#pragma once

#ifdef _WIN32
    #define NOGDI
    #define NOUSER
#endif

#include <PacketManager.hpp>
#include <sockpp/tcp_connector.h>
#include <GamePacket.hpp>
#include <Types.hpp>
#include <memory>

class Multiplayer {
private:
    PacketManager* m_pacman;
    sockpp::tcp_connector m_connector;
    std::vector<std::shared_ptr<GamePacket>> m_packetQueue;

    bool m_canSendNext = true;
    bool m_shouldSendPlayer = true;

public:
    ~Multiplayer();

    bool connect(std::string const& host, in_port_t port);
    void onTick();

    void addToQueue(std::shared_ptr<GamePacket> packet);
    void onBlockChanged(Vec2i pos, uint8_t layer);
    bool shouldSendPlayer() { return m_shouldSendPlayer; }
};