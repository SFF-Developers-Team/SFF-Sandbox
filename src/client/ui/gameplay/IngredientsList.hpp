#pragma once
#include <ui/basic/Frame.hpp>
#include <managers/Recipes.hpp>

class IngredientsList : public Frame {
private:
    const Recipes::Recipe* m_recipe = nullptr;

public:
    IngredientsList();

    void draw();

    void setRecipe(Recipes::Recipe const& recipe) { m_recipe = &recipe; }
};