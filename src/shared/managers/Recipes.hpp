#pragma once
#include <Types.hpp>
#include <array>

class Player;

class Recipes {
public:
    enum class CraftingMethod {
        NONE = 0,
        WORKBENCH,
        FURNACE
    };

    struct Ingredient {
        ItemID id;
        int count;
    };

    struct Recipe {
        ItemID resultId;
        int count;

        const std::vector<Recipes::Ingredient> ingredients;
        CraftingMethod craftingMethod = CraftingMethod::NONE;
    };

    static const auto& Get() { return RecipesTable; }

    /** @brief Crafts item and adds it to player inventory
      * @return Is item crafted successfully
      */
    static bool CraftItem(Player& player, const Recipe& recipe);

private:
    static inline const auto RecipesTable = std::to_array<Recipes::Recipe>({
        // result              count ingredients          workbenchRequired
        {ItemID::PLANKS,        4, {{ItemID::OAK_LOG, 1}}, CraftingMethod::NONE},
        {ItemID::STICK,         4, {{ItemID::PLANKS, 2}}, CraftingMethod::NONE},
        {ItemID::WOODEN_PICKAXE, 1, {{ItemID::STICK, 2}, {ItemID::PLANKS, 3}}, CraftingMethod::WORKBENCH},
        {ItemID::STONE_PICKAXE, 1, {{ItemID::STICK, 2}, {ItemID::COBBLESTONE, 3}}, CraftingMethod::WORKBENCH},
        {ItemID::IRON_PICKAXE,  1, {{ItemID::STICK, 2}, {ItemID::IRON_INGOT, 3}}, CraftingMethod::WORKBENCH},
        {ItemID::GOLDEN_PICKAXE,  1, {{ItemID::STICK, 2}, {ItemID::GOLD_INGOT, 3}}, CraftingMethod::WORKBENCH},
        {ItemID::DIAMOND_PICKAXE, 1, {{ItemID::STICK, 2}, {ItemID::DIAMOND, 3}}, CraftingMethod::WORKBENCH},
        {ItemID::WOODEN_SWORD,  1, {{ItemID::STICK, 1}, {ItemID::PLANKS, 2}}, CraftingMethod::WORKBENCH},
        {ItemID::STONE_SWORD,   1, {{ItemID::STICK, 1}, {ItemID::STONE, 2}}, CraftingMethod::WORKBENCH},
        {ItemID::IRON_SWORD,    1, {{ItemID::STICK, 1}, {ItemID::IRON_INGOT, 2}}, CraftingMethod::WORKBENCH},
        {ItemID::GOLDEN_SWORD,    1, {{ItemID::STICK, 1}, {ItemID::GOLD_INGOT, 2}}, CraftingMethod::WORKBENCH},
        {ItemID::DIAMOND_SWORD, 1, {{ItemID::STICK, 1}, {ItemID::DIAMOND, 2}}, CraftingMethod::WORKBENCH},
        {ItemID::WOODEN_PICKAXE, 1, {{ItemID::STICK, 2}, {ItemID::PLANKS, 3}}, CraftingMethod::WORKBENCH},
        {ItemID::STONE_AXE, 1, {{ItemID::STICK, 2}, {ItemID::COBBLESTONE, 3}}, CraftingMethod::WORKBENCH},
        {ItemID::IRON_AXE,  1, {{ItemID::STICK, 2}, {ItemID::IRON_INGOT, 3}}, CraftingMethod::WORKBENCH},
        {ItemID::GOLDEN_AXE,  1, {{ItemID::STICK, 2}, {ItemID::GOLD_INGOT, 3}}, CraftingMethod::WORKBENCH},
        {ItemID::DIAMOND_AXE, 1, {{ItemID::STICK, 2}, {ItemID::DIAMOND, 3}}, CraftingMethod::WORKBENCH},
        {ItemID::WOODEN_SHOVEL, 1, {{ItemID::STICK, 2}, {ItemID::PLANKS, 1}}, CraftingMethod::WORKBENCH},
        {ItemID::STONE_SHOVEL, 1, {{ItemID::STICK, 2}, {ItemID::STONE, 1}}, CraftingMethod::WORKBENCH},
        {ItemID::IRON_SHOVEL, 1, {{ItemID::STICK, 2}, {ItemID::IRON_INGOT, 1}}, CraftingMethod::WORKBENCH},
        {ItemID::GOLDEN_SHOVEL, 1, {{ItemID::STICK, 2}, {ItemID::GOLD_INGOT, 1}}, CraftingMethod::WORKBENCH},
        {ItemID::DIAMOND_SHOVEL, 1, {{ItemID::STICK, 2}, {ItemID::DIAMOND, 1}}, CraftingMethod::WORKBENCH},

        {ItemID::BOOKSHELF, 1, {{ItemID::PLANKS, 6}, {ItemID::BOOK, 3}}, CraftingMethod::WORKBENCH},
        {ItemID::FURNACE, 1, {{ItemID::COBBLESTONE, 8}}, CraftingMethod::WORKBENCH},
        {ItemID::WORKBENCH, 1, {{ItemID::PLANKS, 4}}, CraftingMethod::NONE},
        {ItemID::CHEST, 1, {{ItemID::PLANKS, 8}}, CraftingMethod::WORKBENCH},
        {ItemID::TORCH, 4, {{ItemID::STICK, 1}, {ItemID::COAL, 1}}, CraftingMethod::WORKBENCH},
        {ItemID::SIGN, 1, {{ItemID::STICK, 1}, {ItemID::PLANKS, 6}}, CraftingMethod::WORKBENCH},
        
        // Furnace
        {ItemID::IRON_INGOT, 1, {{ItemID::IRON_ORE, 1}}, CraftingMethod::FURNACE},
        {ItemID::GOLD_INGOT, 1, {{ItemID::GOLD_ORE, 1}}, CraftingMethod::FURNACE},
        {ItemID::GLASS, 1, {{ItemID::SAND, 1}}, CraftingMethod::FURNACE},
    });
};