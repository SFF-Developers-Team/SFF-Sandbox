#pragma once
#include <vector>
#include <GamePacket.hpp>
#include <platform.hpp>
#include <sockpp/connector.h>
#include <sockpp/acceptor.h>
#include <memory>

#if defined(_WIN32)
    #define ERRWOULDBLOCK WSAEWOULDBLOCK
    #define ERRALREADY WSAEALREADY
#elif defined(__linux__)
    #define ERRWOULDBLOCK EWOULDBLOCK
    #define ERRALREADY EALREADY
#else
    #error "Unknown platform"
#endif


template<typename Socket>
class PacketManager {
private:
    Socket& m_sock;
    
    std::vector<uint8_t> m_writeBuf;
    std::vector<uint8_t> m_readBuf;

    std::size_t m_writeOffset = 0;
    std::size_t m_readOffset = 0;
    std::size_t m_readPacketSize = 0;

public:
    PacketManager(Socket& sock, size_t bufSize) : m_sock(sock), m_readBuf(bufSize) {}

    bool send(std::shared_ptr<GamePacket> packet) { 
        if(!m_writeOffset) {
            auto bytes = packet->serialize();
            uint16_t packetSize = bytes.size();
            m_writeBuf.resize(packetSize + 2);
            std::memcpy(m_writeBuf.data(), &packetSize, 2);
            std::memcpy(m_writeBuf.data() + 2, bytes.data(), packetSize);
        }
        
        auto res = m_sock.write(m_writeBuf.data() + m_writeOffset, m_writeBuf.size() - m_writeOffset);
        m_writeOffset += (res.value() > 0 ? res.value() : 0);

        if(res.error().value() == ERRWOULDBLOCK) {
            return false;
        }

        if(res.is_error()) {
            logE("SEND ERROR {} ({})", res.error_message(), res.error().value());
        }

        m_writeBuf.clear();
        m_writeOffset = 0;

        return true;
    }
    

    std::shared_ptr<GamePacket> recv() {
        auto res = m_sock.read(m_readBuf.data() + m_readOffset, m_readBuf.size() - m_readOffset);
        m_readOffset += (res.value() > 0 ? res.value() : 0);
        
        if(m_readOffset >= 2) m_readPacketSize = *(uint16_t*)(m_readBuf.data());

        if(res.error().value() == ERRWOULDBLOCK || (m_readPacketSize > 0 && m_readOffset < m_readPacketSize + 2)) {
            return nullptr;
        }

        if(res.is_error()) {
            return CREATE_PACKET(SerializedObject::NETWORK_ERROR, res.error_message());
        }

        auto bytes = ByteVector(m_readBuf.begin() + 2, m_readBuf.begin() + m_readOffset);

        m_readOffset = 0;
        m_readPacketSize = 0;

        return CREATE_PACKET(bytes);
    }
};