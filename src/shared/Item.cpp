#include <Item.hpp>

std::string Item::idToString(ItemID id) {
    switch (id) {
        case STICK: return "Stick";
        case WOODEN_PICKAXE: return "Wooden pickaxe";
        case STONE_PICKAXE: return "Stone pickaxe";
        case IRON_PICKAXE: return "Iron pickaxe";
        case GOLDEN_PICKAXE: return "Golden pickaxe";
        case DIAMOND_PICKAXE: return "Diamond pickaxe";
        case WOODEN_SWORD: return "Wooden sword";
        case STONE_SWORD: return "Stone sword";
        case IRON_SWORD: return "Iron sword";
        case GOLDEN_SWORD: return "Golden sword";
        case DIAMOND_SWORD: return "Diamond sword";
    };
}

Item::Item(ItemID id) : ItemBase(id) {}