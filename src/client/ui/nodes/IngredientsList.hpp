#pragma once
#include <ui/nodes/Frame.hpp>
#include <RecipesManager.hpp>

class IngredientsList : public Frame {
private:
    Recipe m_recipe;

public:
    IngredientsList();

    void draw();

    void setRecipe(Recipe const& recipe) { m_recipe = recipe; }
};