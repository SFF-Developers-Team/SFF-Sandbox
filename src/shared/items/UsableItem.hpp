#pragma once

#include "Player.hpp"
#include "Item.hpp"

class UsableItem : public Item {
public:
    UsableItem(int id, uint8_t spriteIndex) : Item(id, spriteIndex) {}
    UsableItem(int id) : Item(id) {}
    UsableItem(UsableItem const& usableitem) : Item(usableitem.m_id, usableitem.m_spriteIndex) {}

    virtual bool OnUse(Player* player) { return false; }
    virtual void Draw() {}
    
    UsableItem& operator=(UsableItem const& usableitem) {
        m_id = usableitem.m_id;
        m_spriteIndex = usableitem.m_spriteIndex;
        return *this;
    }
};