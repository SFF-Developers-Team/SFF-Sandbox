#pragma once
#include <vector>
#include <GamePacket.hpp>
#include <memory>
class PacketManager {
private:
    std::vector<uint8_t> m_writeBuf;
    std::vector<uint8_t> m_readBuf;

    std::size_t m_writeOffset = 0;
    std::size_t m_readOffset = 0;

public:
    PacketManager(std::size_t bufSize);

    template<typename Socket>
    bool send(Socket& sock, std::shared_ptr<GamePacket> packet);
    
    template<typename Socket>
    std::shared_ptr<GamePacket> recv(Socket& sock);
};