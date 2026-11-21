#pragma once
#include <Item.hpp>
#include <Types.hpp>

class World;

class Block {
private:
    static bool m_inited;
    static std::unordered_map<BlockID, std::unique_ptr<Block>> m_blocksDb;

public:
    Block() = default;
    Block(Block& block) = delete;

    virtual ~Block() = default;

    static void init();
    static Block* get(BlockID id);
    static float getDurability(BlockID id);

    virtual BlockID getId() const = 0;

    virtual void onTick(World& world, BlockPosition position) {}
    virtual void onRandomTick(World& world, BlockPosition position) {}

    virtual std::unique_ptr<Item> dropItem(Item* tool) {
        return std::make_unique<Item>(getId());
    }
};