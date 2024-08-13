#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <Logger.hpp>
#include <string.h>
#include <platform.hpp>
using ByteVector = std::vector<uint8_t>;

class SerializedObject {
public:
    enum Header : uint8_t {
        // World headers (also uses in mp)
        PLAYER,
        BLOCK,
        CHUNK,
        WORLD,
        ENTITY,

        // Multiplayer headers
        IDENTIFICATION, DISCONNECT,
        LOAD_CHUNK,  UNLOAD_CHUNK,  CHUNKS,
        LOAD_PLAYER, UNLOAD_PLAYER, PLAYERS,
        BLOCK_PLACE, BLOCK_DESTROY,

        NETWORK_ERROR,
        NULL_PACKET = 0xFF
    };

protected:
    ByteVector m_bytes;
    std::size_t m_offset = 0;
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

    template<>
    void addBytes<std::string>(std::string value) {
        for(char c : value) {
            m_bytes.push_back(c);
        }

        m_bytes.push_back(0x00);
    }

    template<>
    void addBytes<const char*>(const char* value) {
        for(int i = 0; i < strlen(value); i++) {
            m_bytes.push_back(value[i]);
        }

        m_bytes.push_back(0x00);
    }

    void addBytes(ByteVector& byteVec) {
        for(uint8_t byte : byteVec) {
            m_bytes.push_back(byte);
        }
    }

    ByteVector getBytes(std::size_t count) {
        auto ret = ByteVector(m_bytes.begin() + m_offset, m_bytes.begin() + m_offset + count);
        m_offset += count;
        
        return ret;
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

    template<>
    std::string getBytes<std::string>() {
        return getBytes<std::string>("undefined");
    }

    template<>
    const char* getBytes<const char*>() {
        return getBytes<const char*>("undefined");
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

    template<>
    std::string getBytes<std::string>(std::string defaultVal) {
        auto start = m_offset;

        while(m_bytes[m_offset] != 0x00) {
            if(m_offset + 1 > m_bytes.size()) return defaultVal;
            m_offset++;
        }

        return std::string(m_bytes.begin() + start, m_bytes.begin() + m_offset);
    }

    template<>
    const char* getBytes<const char*>(const char* defaultVal) {
        auto start = m_offset;

        // measuring size
        while(m_bytes[m_offset] != 0x00) {
            if(m_offset + 1 > m_bytes.size()) return defaultVal;
            m_offset++;
        }

        char* str = new char[m_offset - start];
        std::strncpy(str, (const char*)&m_bytes[start], m_offset - start);

        return str;
    }

    auto getOffset() {
        return m_offset;
    }
};