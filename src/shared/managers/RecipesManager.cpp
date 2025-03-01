#include <managers/RecipesManager.hpp>
#include <Item.hpp>

RecipesManager::RecipesManager() {
    addRecipe({
        std::make_shared<InventoryItem>(STICK, 4),
        {{PLANKS, 2}}
    });

    addRecipe({
        std::make_shared<InventoryItem>(WOODEN_PICKAXE, 1),
        {{STICK, 2}, {PLANKS, 3}}, true
    });
}