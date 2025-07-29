#pragma once

#include <UsableItem.hpp>
#include "Player.hpp"

class BlockItem : public UsableItem {
public:
    BlockItem(int id) : UsableItem(id) {}
    
    bool OnUse(Player* player) override {
        auto& world = player->GetWorld();
        auto blockPos = player->GetTargetBlock();

        world.SetBlock(blockPos.x, blockPos.y, blockPos.z, (BlockID)m_id);
        
        return true;
    }   
private:
};