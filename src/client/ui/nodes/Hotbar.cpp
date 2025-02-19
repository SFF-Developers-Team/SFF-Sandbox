#include <ui/nodes/Hotbar.hpp>
#include <RenderManager.hpp>
#include <raylib.h>
#include <entity/Player.hpp>

float constexpr cellSize = 32.f;
float constexpr blockSize = cellSize / 2.f;
int constexpr cellsCount = 9;

Hotbar::Hotbar(std::shared_ptr<Player> player) : Frame(), m_player(player) {
    setSize({cellSize * cellsCount + m_border * (cellsCount + 3), cellSize + m_border * 4});
}

void Hotbar::draw() {
    Frame::draw();

    auto inventory = m_player->getInventory();
    auto selected = m_player->getSelectedIndex();

    for (auto i = 0; i < cellsCount; i++) {
        Rectf cell = {m_border * 2 + cellSize * i + m_border * i, m_border * 2, cellSize, cellSize};
        Rectf block = {cell.x + (cell.width - blockSize) / 2, cell.y + (cell.height - blockSize) / 2, blockSize, blockSize};

        RenderManager::drawFrame(cell, m_color, m_border);
        
        if(inventory[i] != nullptr) {
            RenderManager::renderBlock(block, inventory[i]);
        }

        if(i == selected) {
            RenderManager::drawRectLines(cell, COL_WHITE, m_border);
        }
    }
}