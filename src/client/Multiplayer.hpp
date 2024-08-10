#pragma once
#define NOGDI
#define NOUSER
#include <sockpp/tcp_connector.h>
#include <GamePacket.hpp>
#include <memory>

class Multiplayer {
private:
    sockpp::tcp_connector m_connector;
    std::vector<std::shared_ptr<GamePacket>> m_packetQueue;

    bool m_canSendNext = true;
    bool m_shouldSendPlayer = true;

public:
    ~Multiplayer();

    bool connect(std::string const& host, in_port_t port);
    void update();

    bool send(std::shared_ptr<GamePacket> packet);
    std::shared_ptr<GamePacket> read();
    std::shared_ptr<GamePacket> read(size_t n);
    std::shared_ptr<GamePacket> read(ByteVector& buf);

    void addToQueue(std::shared_ptr<GamePacket> packet);

    bool shouldSendPlayer() { return m_shouldSendPlayer; }
};