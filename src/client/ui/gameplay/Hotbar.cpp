#include <raylib-cpp.hpp>
#include <ui/gameplay/Hotbar.hpp>
#include <managers/RenderManager.hpp>
#include <managers/ResourceManager.hpp>
#include <entity/LocalPlayer.hpp>
#include <world/Tile.hpp>

float constexpr cellSize = 26.f;
float constexpr tileSize = 16.f;
int constexpr cellsCount = 9;

Hotbar::Hotbar(std::shared_ptr<LocalPlayer> player, MiniFunction<void()> const& inventoryCallback) : Frame(), m_player(player), m_invCallback(inventoryCallback) {
    setSize({cellSize * (cellsCount + 1) + m_border * (cellsCount + 4), cellSize + m_border * 4});
}

void Hotbar::draw() {
    Frame::draw();

    auto selected = m_player->getSelectedItem();

    for (auto i = 0; i <= cellsCount; i++) {
        raylib::Rectangle cellRect = {m_border * 2 + cellSize * i + m_border * i, m_border * 2, cellSize, cellSize};
        raylib::Rectangle itemRect = {cellRect.x + (cellRect.width - tileSize) / 2, cellRect.y + (cellRect.height - tileSize) / 2, tileSize, tileSize};
        auto& slot = m_player->getItem(i);

        RenderManager::drawFrame(cellRect, m_color, m_border);

        if(i == cellsCount) {
            auto& font = ResourceManager::Get().GetFont("boldfont");
            float fontSize = static_cast<float>(font.baseSize);
            auto bounds = font.MeasureText("...", fontSize, 1.f);
            font.DrawText("...", {cellRect.x + (cellRect.width - bounds.x) / 2.f, cellRect.y + (cellRect.height - bounds.y) / 2.f}, fontSize, 1.f, raylib::Color::White());
            break;
        }

        if(slot.item != nullptr) {
            RenderManager::renderInventoryItem(itemRect, *slot.item, slot.count);
        }

        if(i == selected) {
            RenderManager::drawRectLines(cellRect, raylib::Color::White(), m_border);
        }
    }
}

void Hotbar::update() {
    for (int i = 0; i < 9; i++) {
        if (raylib::Keyboard::IsKeyDown(KEY_ONE + i)) {
            m_player->setSelectedItem(i);
        }
    }

    auto wheel = raylib::Mouse::GetWheelMove();
    auto mouse = getLocalMousePosition();
    auto index = m_player->getSelectedItem();

    if(!raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL) && wheel != 0.f) {
        (wheel > 0.f ? index-- : index++);
        
        if(index < 0) index = 8;
        if(index >= 9) index = 0;
    
        m_player->setSelectedItem(index);
    }

    for (auto i = 0; i <= cellsCount; i++) {
        raylib::Rectangle cell = {m_border * 2 + cellSize * i + m_border * i, m_border * 2, cellSize, cellSize};
        
        if(cell.CheckCollision(mouse) && raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
            if(i == cellsCount) {
                m_invCallback();
                break;
            }

            m_player->setSelectedItem(i);
        }
    }
}