#pragma once
#include <inventory/InventoryItem.hpp>
#include <list>
#include <unordered_map>

struct RecipeIngredient {
    ItemID id;
    uint16_t count;
};

struct Recipe {
    std::shared_ptr<InventoryItem> result;
    std::vector<RecipeIngredient> ingredients;
    bool workbenchRequired = false;
};

class RecipesManager {
private:
    std::vector<Recipe> m_recipes;

public:
    static auto get() {
        static auto rm = new RecipesManager();
        return rm;
    }

    RecipesManager();

    void addRecipe(Recipe recipe) { m_recipes.push_back(recipe); }

    auto& getRecipes() { return m_recipes; }
};