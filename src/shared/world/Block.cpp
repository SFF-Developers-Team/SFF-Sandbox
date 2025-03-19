#include <world/Block.hpp>
#include <world/blocks/Leaves.hpp>
#include <world/blocks/Stone.hpp>
#include <world/blocks/Grass.hpp>
#include <world/blocks/CoalOre.hpp>
#include <world/blocks/IronOre.hpp>
#include <world/blocks/GoldOre.hpp>
#include <world/blocks/DiamondOre.hpp>
#include <MiniFunction.hpp>
#include <cassert>

std::shared_ptr<Block> Block::create(ItemID id) {
    switch (id) {
        case LEAVES: return std::make_shared<Leaves>();
        case STONE: return std::make_shared<Stone>();
        case GRASS: return std::make_shared<Grass>();
        case COAL_ORE: return std::make_shared<CoalOre>();
        case IRON_ORE: return std::make_shared<IronOre>();
        case GOLD_ORE: return std::make_shared<GoldOre>();
        case DIAMOND_ORE: return std::make_shared<DiamondOre>();
        default: return std::make_shared<Block>(id);
    }
}

std::shared_ptr<Block> Block::create(Item& block) {
    auto ret = Block::create(block.getID());
    ret->setTags(block.getTags());

    return ret;
}

Block::Block(ItemID id) : Item(id) {
    m_header = BLOCK;
}

Block::Block(Block& block) : Item(reinterpret_cast<Item&>(block)) {
    m_header = BLOCK;
}

Block::Block(InventoryItem& item) : Block(item.getID()) {
    m_tags = item.getTags();
}

std::shared_ptr<InventoryItem> Block::dropItem(std::shared_ptr<Item> tool) {
    return std::make_shared<InventoryItem>(*this, 1);
}