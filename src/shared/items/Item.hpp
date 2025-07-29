#pragma once

#include <cstdint>

class Item {
protected:
    int m_id;
    uint8_t m_spriteIndex = 0;
public:
    Item(int id, uint8_t spriteIndex = 0) : m_id(id), m_spriteIndex(spriteIndex) {}
    Item(Item const& item) : m_id(item.m_id), m_spriteIndex(item.m_spriteIndex) {} 
    
    int GetID() { return m_id; }
    
    uint8_t GetSpriteIndex() {
        return m_spriteIndex;
    }

    virtual ~Item() = default;
    
    Item& operator=(Item const& item) {
        m_id = item.m_id;
        return *this;
    }
};