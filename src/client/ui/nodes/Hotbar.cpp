#include <ui/nodes/Hotbar.hpp>
#include <RenderManager.hpp>
#include <raylib.h>
#include <entity/Player.hpp>

float constexpr cellSize = 32.f;
float constexpr blockSize = cellSize / 2.f;
int constexpr cellsCount = 9;

Hotbar::Hotbar(std::shared_ptr<Player> player, MiniFunction<void()> const& inventoryCallback) : Frame(), m_player(player), m_invCallback(inventoryCallback) {
    setSize({cellSize * (cellsCount + 1) + m_border * (cellsCount + 4), cellSize + m_border * 4});
}

void Hotbar::draw() {
    Frame::draw();

    auto inventory = m_player->getInventory();
    auto selected = m_player->getSelectedIndex();

    for (auto i = 0; i <= cellsCount; i++) {
        Rectf cell = {m_border * 2 + cellSize * i + m_border * i, m_border * 2, cellSize, cellSize};
        Rectf block = {cell.x + (cell.width - blockSize) / 2, cell.y + (cell.height - blockSize) / 2, blockSize, blockSize};

        RenderManager::drawFrame(cell, m_color, m_border);

        if(i == cellsCount) {
            RenderManager::drawText("boldfont", "...", {cell.x + cell.width / 2, cell.y + cell.width / 2}, COL_WHITE, 0.f, {0.5f, 0.5f});
            break;
        }
        
        if(inventory[i] != nullptr) {
            RenderManager::renderBlock(block, inventory[i]);
        }

        if(i == selected) {
            RenderManager::drawRectLines(cell, COL_WHITE, m_border);
        }
    }
}

void Hotbar::update() {
    for (int i = 0; i < 9; i++) {
        if (IsKeyDown(KEY_ONE + i)) {
            m_player->setSelectedIndex(i);
        }
    }

    auto wheel = GetMouseWheelMove();
    auto mouse = getLocalMousePosition();
    auto index = m_player->getSelectedIndex();

    if(!IsKeyDown(KEY_LEFT_CONTROL) && wheel != 0.f) {
        (wheel > 0.f ? index-- : index++);
        
        if(index < 0) index = 8;
        if(index >= 9) index = 0;
    
        m_player->setSelectedIndex(index);
    }

    for (auto i = 0; i <= cellsCount; i++) {
        Rectf cell = {m_border * 2 + cellSize * i + m_border * i, m_border * 2, cellSize, cellSize};
        
        if(cell.contains(mouse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if(i == cellsCount) {
                m_invCallback();
                break;
            }

            m_player->setSelectedIndex(i);
        }
    }
}