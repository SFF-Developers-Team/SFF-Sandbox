#include <ui/gameplay/IngredientsList.hpp>
#include <RenderManager.hpp>

float const cellSize = 24.f;
float const blockSize = 16.f;

IngredientsList::IngredientsList() : Frame() {
    setHeight(cellSize + m_border * 4);
}

void IngredientsList::draw() {
    Frame::draw();
    
    if(m_recipe.result != nullptr) {
        for (auto i = 0; i < m_recipe.ingredients.size(); i++) {
            auto ingredient = m_recipe.ingredients[i];
            Rectf cellRect = {m_border * 2 + cellSize * i, m_border * 2, cellSize, cellSize};
            Rectf itemRect = {cellRect.x + (cellRect.width - blockSize) / 2, cellRect.y + (cellRect.height - blockSize) / 2, blockSize, blockSize};

            RenderManager::drawFrame(cellRect, m_color, m_border);
            RenderManager::renderInventoryItem(itemRect, std::make_shared<InventoryItem>(ingredient.type, ingredient.id, ingredient.count));
        }
    }
}