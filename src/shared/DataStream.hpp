#pragma once
#include <string>
#include <algorithm>
#include <cstring>
#include <Types.hpp>

class DataStream : public ByteVector {
protected:
    std::size_t m_offset = 0;

public:
    DataStream() : ByteVector() {}
    DataStream(size_t size) : ByteVector(size) {}
    DataStream(ByteVector const& vec) : ByteVector(vec) {}

    template <class InputIt>
    DataStream(InputIt first, InputIt last) : ByteVector(first, last) {}

    template<typename T>
    void add(T value) {
        for(int i = 0; i < sizeof(T); i++) {
            push_back(((uint8_t*)&value)[i]);
        }

        m_offset += sizeof(T);
    }

    template<typename T>
    void add(std::vector<T> const& vec) {
        add<uint16_t>(vec.size());

        for (auto& el : vec) {
            add<T>(el);
        }
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

    template<typename T>
    std::vector<T> get(std::initializer_list<T> const& def) {
        auto count = get<uint16_t>(0);

        if (!count) {
            return def;
        }

        std::vector<T> vec(count);

        while (count-- > 0) {
            vec.push_back(get<T>());
        }

        return vec;
    }

    std::string get(std::string const& defaultString) {
        return get(defaultString.c_str());
    }

    const char* get(const char* defaultVal = "unknown") {
        auto len = std::strlen((char*)data() + m_offset);

        if(m_offset + len > size()) return defaultVal;

        auto ret = new char[len + 1];
        std::memset(ret, 0, len + 1);
        std::copy(begin() + m_offset, begin() + m_offset + len, ret);

        m_offset += len;

        return ret;
    }

    /// @brief Ignore offset
    template<typename T>
    T getI(T defaultVal = T()) {
        auto ret = get(defaultVal);
        m_offset -= sizeof(ret);
        return ret;
    }

    std::size_t offset() { return m_offset; }
    
    /// @return All bytes of serialized object
    ByteVector bytes() { return ByteVector(begin(), end()); }

    /// @brief Reset offset 
    void reset() { m_offset = 0; }
};