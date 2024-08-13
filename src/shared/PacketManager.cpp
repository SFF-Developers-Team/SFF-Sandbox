#include <PacketManager.hpp>
#include <platform.hpp>

PacketManager::PacketManager(size_t bufSize) : m_readBuf(bufSize) {}

template<typename Socket>
bool PacketManager::send(Socket& sock, std::shared_ptr<GamePacket> packet) { 
    if(!m_writeOffset) {
        auto bytes = packet->serialize();
        m_writeBuf.resize(bytes.size());
        std::memcpy(m_writeBuf.data(), bytes.data(), bytes.size());
    }
    
    auto res = sock.write(m_writeBuf.data() + m_writeOffset, m_writeBuf.size() - m_writeOffset);

    if(res.value() < packet->getSize()) {
        m_writeOffset = res.value();
        return false;
    }

    return true;
}

template<typename Socket>
std::shared_ptr<GamePacket> PacketManager::recv(Socket& sock) {
    auto res = sock.read(m_readBuf.data(), m_readBuf.size());
    m_readOffset += res.value();

    if(res.error().value() == EALREADY || res.error().value() == EWOULDBLOCK) {
        return nullptr;
    }

    if(!res.is_ok()) {
        return CREATE_PACKET(SerializedObject::SERVER_ERROR, res.error_message());
    }

    auto bytes = ByteVector(m_readOffset);
    std::memcpy(bytes.data(), m_readBuf.data(), m_readOffset);

    m_readOffset = 0;

    return CREATE_PACKET(bytes);
}