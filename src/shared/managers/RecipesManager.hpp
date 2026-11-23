#pragma once
#include <list>
#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_map>

class Player;

struct RecipeIngredient {
    int id;
    uint16_t count;
};

struct Recipe {
    int resultId;
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