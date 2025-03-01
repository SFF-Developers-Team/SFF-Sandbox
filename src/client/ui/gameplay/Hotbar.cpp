#include <ui/gameplay/Hotbar.hpp>
#include <managers/RenderManager.hpp>
#include <raylib.h>
#include <entity/Player.hpp>
#include <world/Block.hpp>

float constexpr cellSize = 26.f;
float constexpr blockSize = 16.f;
int constexpr cellsCount = 9;

Hotbar::Hotbar(std::shared_ptr<Player> player, MiniFunction<void()> const& inventoryCallback) : Frame(), m_player(player), m_invCallback(inventoryCallback) {
    setSize({cellSize * (cellsCount + 1) + m_border * (cellsCount + 4), cellSize + m_border * 4});
}

void Hotbar::draw() {
    Frame::draw();

    auto selected = m_player->getSelectedItem();

    for (auto i = 0; i <= cellsCount; i++) {
        Rectf cellRect = {m_border * 2 + cellSize * i + m_border * i, m_border * 2, cellSize, cellSize};
        Rectf itemRect = {cellRect.x + (cellRect.width - blockSize) / 2, cellRect.y + (cellRect.height - blockSize) / 2, blockSize, blockSize};
        auto item = m_player->getItem(i);

        RenderManager::drawFrame(cellRect, m_color, m_border);

        if(i == cellsCount) {
            RenderManager::drawText("boldfont", "...", {cellRect.x + cellRect.width / 2, cellRect.y + cellRect.width / 2}, COL_WHITE, 0.f, {0.5f, 0.5f});
            break;
        }

        if(item != nullptr) {
            RenderManager::renderInventoryItem(itemRect, item);
        }

        if(i == selected) {
            RenderManager::drawRectLines(cellRect, COL_WHITE, m_border);
        }
    }
}

void Hotbar::update() {
    for (int i = 0; i < 9; i++) {
        if (IsKeyDown(KEY_ONE + i)) {
            m_player->setSelectedItem(i);
        }
    }

    auto wheel = GetMouseWheelMove();
    auto mouse = getLocalMousePosition();
    auto index = m_player->getSelectedItem();

    if(!IsKeyDown(KEY_LEFT_CONTROL) && wheel != 0.f) {
        (wheel > 0.f ? index-- : index++);
        
        if(index < 0) index = 8;
        if(index >= 9) index = 0;
    
        m_player->setSelectedItem(index);
    }

    for (auto i = 0; i <= cellsCount; i++) {
        Rectf cell = {m_border * 2 + cellSize * i + m_border * i, m_border * 2, cellSize, cellSize};
        
        if(cell.contains(mouse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if(i == cellsCount) {
                m_invCallback();
                break;
            }

            m_player->setSelectedItem(i);
        }
    }
}