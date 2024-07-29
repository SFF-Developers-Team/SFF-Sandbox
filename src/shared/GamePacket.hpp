#pragma once
#include <SerializedObject.hpp>

#define MP_BUF_SIZE (1024 * 64) // 64kb

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

    ByteVector& serialize() {
        return m_bytes;
    }

    auto getSize() {
        return m_bytes.size();
    }
};