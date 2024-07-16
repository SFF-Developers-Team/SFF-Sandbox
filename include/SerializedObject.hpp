#pragma once
#include <vector>

using ByteVector = std::vector<uint8_t>;

class SerializedObject {
public:
    enum Header : unsigned char {
        PLAYER,
        BLOCK,
        CHUNK,
        WORLD
    };

protected:
    ByteVector m_bytes;
    size_t m_offset = 0;
    Header m_header;

public:
    inline ByteVector& serialize() {
        m_bytes.clear();
        addBytes(m_header);

        return m_bytes;
    }

    inline int deserialize(ByteVector& bytes) {
        m_bytes = bytes;
        m_offset = 0;

        m_header = getBytes<Header>();

        return m_offset;
    }

    template<typename T>
    void addBytes(T value) {
        for(int i = 0; i < sizeof(T); i++) {
            m_bytes.push_back(((uint8_t*)&value)[i]);
        }
    }

    void addBytes(ByteVector& byteVec) {
        for(uint8_t byte : byteVec) {
            m_bytes.push_back(byte);
        }
    }

    template<typename T>
    T getBytes() {
        size_t sz = sizeof(T);
        T t = T();
        
        if (m_offset + sz > m_bytes.size()) return t;

        auto data = m_bytes.data();
        t = *(T*)&data[m_offset];
        m_offset += sz;

        return t;
    }

    template<typename T>
    T getBytes(T defaultVal) {
        size_t sz = sizeof(T);
        T t(defaultVal);
        
        if (m_offset + sz > m_bytes.size()) return t;

        t = *(T*)&m_bytes[m_offset];
        m_offset += sz;

        return t;
    }
};