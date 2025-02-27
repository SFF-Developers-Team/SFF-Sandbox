#include <RecipesManager.hpp>
#include <Item.hpp>

RecipesManager::RecipesManager() {
    addRecipe({
        std::make_shared<InventoryItem>(INV_ITEM, STICK, 4),
        {{INV_BLOCK, PLANKS, 2}}
    });

    addRecipe({
        std::make_shared<InventoryItem>(INV_ITEM, WOODEN_PICKAXE, 1),
        {{INV_ITEM, STICK, 2}, {INV_BLOCK, PLANKS, 3}}, true
    });
}