#include <managers/RecipesManager.hpp>
#include <Item.hpp>
#include <entity/Player.hpp>

RecipesManager::RecipesManager() {
    addRecipe({
        .resultId = (int)TileID::PLANKS, 
        .count = 4,
        .ingredients = {{(int)TileID::OAK_LOG, 1}},
        .workbenchRequired = false
    });

    addRecipe({
        .resultId = (int)ItemID::STICK, 
        .count = 4,
        .ingredients = {{(int)TileID::PLANKS, 2}},
        .workbenchRequired = false
    });

    addRecipe({
        .resultId = (int)ItemID::WOODEN_PICKAXE,
        .count = 1,
        .ingredients = {{(int)ItemID::STICK, 2}, {(int)TileID::PLANKS, 3}}, 
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