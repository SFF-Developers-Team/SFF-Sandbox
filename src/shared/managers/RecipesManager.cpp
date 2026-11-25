#include <managers/RecipesManager.hpp>
#include <Item.hpp>
#include <entity/Player.hpp>

RecipesManager::RecipesManager() {
    addRecipe({
        .resultId = ItemID::PLANKS, 
        .count = 4,
        .ingredients = {{ItemID::OAK_LOG, 1}},
        .workbenchRequired = false
    });

    addRecipe({
        .resultId = ItemID::STICK, 
        .count = 4,
        .ingredients = {{ItemID::PLANKS, 2}},
        .workbenchRequired = false
    });

    addRecipe({
        .resultId = ItemID::WOODEN_PICKAXE,
        .count = 1,
        .ingredients = {{ItemID::STICK, 2}, {ItemID::PLANKS, 3}}, 
        .workbenchRequired = true
    });
}

bool RecipesManager::craftItem(Player& player, Recipe& recipe) {
    for (auto& item : recipe.ingredients) {
        auto playerItem = player.findItem(item.id);

        if (playerItem == nullptr || playerItem->count < item.count) {
            return false;
        }
    }

    for (auto& item : recipe.ingredients) {
        player.deleteItem(item.id, item.count);
    }

    player.addItem(std::make_unique<Item>(recipe.resultId), recipe.count);
    
    return true;
}