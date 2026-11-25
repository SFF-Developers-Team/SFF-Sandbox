#pragma once
#include <list>
#include <vector>
#include <memory>
#include <unordered_map>
#include <Types.hpp>

class Player;

struct RecipeIngredient {
    ItemID id;
    int count;
};

struct Recipe {
    ItemID resultId;
    int count;

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

    bool craftItem(Player& player, Recipe& recipe);
};