#include <managers/RecipesManager.hpp>
#include <Item.hpp>
#include <entity/SimplePlayer.hpp>

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

bool RecipesManager::craftItem(std::shared_ptr<SimplePlayer> player, Recipe& recipe) {
    for (auto& item : recipe.ingredients) {
        auto playerItem = player->findItem(item.id);

        if (playerItem == nullptr || playerItem->getCount() < item.count) {
            return false;
        }
    }

    for (auto& item : recipe.ingredients) {
        player->deleteItem(item.id, item.count);
    }

    player->addItem(std::make_shared<InventoryItem>(*recipe.result));
    
    return true;
}