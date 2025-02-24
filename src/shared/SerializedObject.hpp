#pragma once
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <string.h>
#include <mutex>

using ByteVector = std::vector<uint8_t>;

class SerializedObject : public ByteVector {
public:
    enum Header : uint8_t {
        // World headers (also uses in mp)
        PLAYER,
        BLOCK,
        CHUNK,
        WORLD,
        ENTITY,
        INVENTORY_ITEM,

        // Multiplayer headers
        IDENTIFICATION, DISCONNECT,
        LOAD_CHUNK,  UNLOAD_CHUNK, CHUNKS,
        LOAD_PLAYER, UNLOAD_PLAYER,
        BLOCK_PLACE, BLOCK_DESTROY,

        NETWORK_ERROR, ARRAY,
        NULL_PACKET = 0xFF
    };

    using ByteVector::data;
    using ByteVector::size;

protected:
    std::size_t m_offset = 0;
    Header m_header;

public:
    SerializedObject() {}
    SerializedObject(Header header) : m_header(header) {}

    virtual ByteVector serialize() {
        clear();
        add(m_header);

        return bytes();
    }

    virtual size_t deserialize(ByteVector const& bytes) {
        resize(bytes.size());
        std::copy(bytes.begin(), bytes.end(), begin());
        
        m_offset = 0;
        m_header = get<Header>();

        return m_offset;
    }

    template<typename T>
    void add(T value) {
        for(int i = 0; i < sizeof(T); i++) {
            push_back(((uint8_t*)&value)[i]);
        }

        m_offset += sizeof(T);
    }

    void add(const char* str, size_t size) {
        resize(m_offset + size);
        std::copy(str, str + size, &at(m_offset));

        m_offset += size;

        if(at(m_offset - 1) != 0) {
            push_back(0);
            m_offset++;
        }
    }

    /// @brief Append other ByteVector to serialized object
    void add(ByteVector const& bytes) {
        // wtf why it is dont work
        // if(m_offset + bytes.size() > size()) {
        //     resize(m_offset + bytes.size());
        // }
        //
        // std::copy(bytes.begin, bytes.end(), (begin() + m_offset));

        for(auto& byte : bytes) {
            push_back(byte);
        }

        m_offset += bytes.size();
    }

    void add(std::string const& str) {
        return add(str.c_str(), str.size());
    }

    /// @brief Get N-count bytes from serialized object
    ByteVector getN(std::size_t count) {
        if(m_offset + count > size()) {
            count = size() - m_offset;
        }

        auto ret = ByteVector(begin() + m_offset, begin() + m_offset + count);
        m_offset += ret.size();
        return ret;
    }

    template<typename T>
    T get(T defaultVal = T()) {
        size_t sz = sizeof(T);
        T t(defaultVal);
        
        if (m_offset + sz > size()) return t;

        t = *(T*)&at(m_offset);
        m_offset += sz;

        return t;
    }

    /// @brief Ignore offset
    template<typename T>
    T getI(T defaultVal = T()) {
        auto ret = get(defaultVal);
        m_offset -= sizeof(ret);
        return ret;
    }

    const char* get(const char* defaultVal) {
        auto len = std::strlen((char*)data() + m_offset);

        if(m_offset + len > size()) return defaultVal;

        auto ret = new char[len + 1];
        std::memset(ret, 0, len + 1);
        std::copy(begin() + m_offset, begin() + m_offset + len, ret);

        m_offset += len;

        return ret;
    }

    std::size_t offset() { return m_offset; }
    
    /// @return All bytes of serialized object
    ByteVector bytes() { return ByteVector(begin(), end()); }

    uint32_t hash() {
        std::size_t seed = size();
        
        for(auto i = begin(); i != end(); i++) {
            seed ^= *i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        return seed;
    }
    
    /// @brief Reset offset 
    void reset() { m_offset = 0; }
};