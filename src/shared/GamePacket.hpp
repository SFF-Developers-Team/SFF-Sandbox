#pragma once
#include <SerializedObject.hpp>

#define MP_BUF_SIZE (65344)
#define CREATE_PACKET std::make_shared<GamePacket>

class GamePacket : public SerializedObject {
public:
    template<typename... Args>
    GamePacket(Header header, Args const&... arg) {
        addBytes(header);
        addBytes(arg...);
    }

    GamePacket(Header header, ByteVector& bytes) {
        addBytes(header);
        addBytes(bytes);
    }

    GamePacket(ByteVector& bytes) {
        addBytes(bytes);
    }

    GamePacket(Header header) {
        addBytes(header);
    }

    ByteVector& serialize() override { return m_bytes; }
    
    auto resetOffset() { m_offset = 0; }
    auto getSize() { return m_bytes.size(); }
};