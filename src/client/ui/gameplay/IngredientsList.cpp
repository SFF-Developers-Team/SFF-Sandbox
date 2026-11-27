#include <ui/gameplay/IngredientsList.hpp>
#include <managers/RenderManager.hpp>
#include <Item.hpp>

float const cellSize = 24.f;
float const tileSize = 16.f;

IngredientsList::IngredientsList() : Frame() {
    setHeight(cellSize + m_border * 4);
}

void IngredientsList::draw() {
    Frame::draw();

    if (m_recipe == nullptr) {
        return;
    }
    
    for (auto i = 0; i < m_recipe->ingredients.size(); i++) {
        auto& ingredient = m_recipe->ingredients.at(i);
        raylib::Rectangle cellRect{m_border * 2 + cellSize * i, m_border * 2, cellSize, cellSize};
        raylib::Rectangle itemRect{cellRect.x + (cellRect.width - tileSize) / 2, cellRect.y + (cellRect.height - tileSize) / 2, tileSize, tileSize};

        RenderManager::drawFrame(cellRect, m_color, m_border);
        RenderManager::renderInventoryItem(itemRect, ingredient.id, ingredient.count);
    }
}