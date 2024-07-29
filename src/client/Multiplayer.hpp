#pragma once
#define NOGDI
#define NOUSER
#include <sockpp/tcp_connector.h>
#include <GamePacket.hpp>

class Multiplayer {
private:
    sockpp::tcp_connector m_connector;
    std::vector<GamePacket*> m_packetQueue;

public:
    bool connect(std::string const& host, in_port_t port);
    void thread();

    void send(GamePacket* packet);
    GamePacket* read();
    GamePacket* read(size_t n);
    GamePacket* read(ByteVector& buf);

    void addToQueue(GamePacket* packet);
};