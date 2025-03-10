#include <world/Block.hpp>
#include <cassert>
#include <world/blocks/Leaves.hpp>
#include <world/blocks/Stone.hpp>
#include <world/blocks/Grass.hpp>
#include <world/blocks/CoalOre.hpp>
#include <world/blocks/IronOre.hpp>
#include <world/blocks/GoldOre.hpp>
#include <world/blocks/DiamondOre.hpp>

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

Block::Block(ItemID id) : Item(id), m_x(0), m_y(0), m_layer(1) {
    m_header = BLOCK;
}

Block::Block(Block& block) : Block(block.m_id) {
    m_x = block.m_x;
    m_y = block.m_y; 
    m_layer = block.m_layer;
    m_tags = block.m_tags;
}

Block::Block(InventoryItem& item) : Block(item.getID()) {
    m_tags = item.getTags();
}

void Block::setPos(int32_t x, int32_t y, uint8_t layer) {
    m_x = x;
    m_y = y;
    m_layer = layer;
}

Rectf Block::getHitbox() {
    if(m_layer == 0 || m_id == AIR || (hasTag(TAG_GHOST) && getTag<bool>(TAG_GHOST))) {
        return Rectf {0.0f, 0.0f, 0.0f, 0.0f};
    }

    return Rectf {static_cast<float>(m_x), static_cast<float>(m_y), 1.0f, 1.0f};
}

float Block::getDurability() {
    switch (getMaterial()) {
        default:
        case MATERIAL_WOOL:
        case MATERIAL_DIRT: return 10.f;
        case MATERIAL_WOOD: return 25.f;
        case MATERIAL_STONE: return 50.f;
    }
}

std::shared_ptr<InventoryItem> Block::dropItem(std::shared_ptr<Item> tool) {
    return std::make_shared<InventoryItem>(*this, 1);
}