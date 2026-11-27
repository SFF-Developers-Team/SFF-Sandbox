#include "Recipes.hpp"
#include <Item.hpp>
#include <entity/Player.hpp>

bool Recipes::CraftItem(Player& player, const Recipe& recipe) {
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