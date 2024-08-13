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

class PacketManager {
private:
    std::vector<uint8_t> m_writeBuf;
    std::vector<uint8_t> m_readBuf;

    std::size_t m_writeOffset = 0;
    std::size_t m_readOffset = 0;

public:
    PacketManager(size_t bufSize) : m_readBuf(bufSize) {}
    
    template<typename Socket>
    bool send(Socket& sock, std::shared_ptr<GamePacket> packet) { 
        if(!m_writeOffset) {
            auto bytes = packet->serialize();
            m_writeBuf.resize(bytes.size());
            std::memcpy(m_writeBuf.data(), bytes.data(), bytes.size());
        }
        
        auto res = sock.write(m_writeBuf.data() + m_writeOffset, m_writeBuf.size() - m_writeOffset);

        m_writeOffset += res.value();

        if(m_writeOffset < m_writeBuf.size()) {
            return false;
        }

        m_writeBuf.clear();
        m_writeOffset = 0;

        return true;
    }
    
    template<typename Socket>
    std::shared_ptr<GamePacket> recv(Socket& sock) {
        auto res = sock.read(m_readBuf.data() + m_readOffset, m_readBuf.size() - m_readOffset);
        m_readOffset += res.value();

        if(res.error().value() == ERRALREADY || res.error().value() == ERRWOULDBLOCK) {
            return nullptr;
        }

        if(res.is_error()) {
            return CREATE_PACKET(SerializedObject::NETWORK_ERROR, res.error_message());
        }

        auto bytes = ByteVector(m_readOffset);
        std::memcpy(bytes.data(), m_readBuf.data(), m_readOffset);

        m_readOffset = 0;

        return CREATE_PACKET(bytes);
    }
};