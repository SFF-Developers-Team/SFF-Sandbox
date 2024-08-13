#pragma once
#include <vector>
#include <GamePacket.hpp>

class PacketManager {
private:
    std::vector<uint8_t> m_writeBuf;
    std::vector<uint8_t> m_readBuf;

    size_t m_writeOffset = 0;
    size_t m_readOffset = 0;

public:
    PacketManager(size_t bufSize);

    template<typename Socket>
    bool send(Socket& sock, std::shared_ptr<GamePacket> packet);
    
    template<typename Socket>
    std::shared_ptr<GamePacket> recv(Socket& sock);
};