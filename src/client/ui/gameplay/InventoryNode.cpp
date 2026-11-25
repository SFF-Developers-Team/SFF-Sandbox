#include <raylib-cpp.hpp>
#include <ui/gameplay/InventoryNode.hpp>
#include <managers/RenderManager.hpp>

float constexpr cellSize = 24.f;
float constexpr tileSize = 16.f;
int const columnCount = 9;
int const rowCount = 4;

InventoryNode::InventoryNode(std::shared_ptr<LocalPlayer> player) : Frame(), m_player(player) {
    setSize({
        cellSize * columnCount + m_border * (columnCount + 3), 
        cellSize * rowCount + m_border * (rowCount + 4)
    });
    m_selected = nullptr;
}

raylib::Rectangle InventoryNode::getCellPosition(int i) {
    auto cellX = i % columnCount;
    auto cellY = i / columnCount;

    raylib::Rectangle cell = {
        m_border * 2 + cellSize * cellX + m_border * cellX, 
        m_border * 2 + cellSize * cellY + m_border * cellY + (cellY > 0 ? m_border : 0), 
        cellSize, cellSize
    };

    return cell;
}

void InventoryNode::draw() {
    Frame::draw();

    for (auto i = 0; i < m_player->getSlotsCount(); i++) {
        raylib::Rectangle cellRect = getCellPosition(i);
        raylib::Rectangle itemRect{cellRect.GetPosition() + (cellRect.GetSize() - tileSize) / 2, Vector2{tileSize, tileSize}};
        raylib::Vector2 mouse = getLocalMousePosition();

        RenderManager::drawFrame(cellRect, m_color, m_border);

        auto& slot = m_player->getItem(i);
            
        if (slot.item != nullptr && m_selected != &slot) {
            RenderManager::renderInventoryItem(itemRect, slot.item->getID(), slot.count);
        }

        if (cellRect.CheckCollision(mouse)) {
            cellRect.Draw(raylib::Color{255, 255, 255, 64});
        }
    }

    if(m_selected != nullptr) {
        auto mouse = getLocalMousePosition();

        raylib::Rectangle tileRect = {mouse.x - tileSize / 2, mouse.y - tileSize / 2, tileSize, tileSize};
        RenderManager::renderInventoryItem(tileRect, m_selected->item->getID(), m_selected->count);
    }
}

void InventoryNode::update() {
    auto mouse = getLocalMousePosition();

    for (auto i = 0; i < columnCount * rowCount; i++) {
        auto cell = getCellPosition(i);
        auto lclick = raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON);
        auto rclick = raylib::Mouse::IsButtonPressed(MOUSE_RIGHT_BUTTON);
        auto& slot = m_player->getItem(i);
        auto& inv = m_player->getInventory();

        if(lclick && cell.CheckCollision(mouse) && m_selected == nullptr && slot.item != nullptr) {
            m_selected = &inv[i];

            continue;
        }

        if(lclick && cell.CheckCollision(mouse) && m_selected != nullptr) {
            if (slot.item != nullptr) {
                std::swap(slot.item, m_selected->item);
                std::swap(slot.count, m_selected->count);
                continue;
            }

            slot.item = std::move(m_selected->item);
            slot.count = m_selected->count;
            
            m_selected = nullptr;
            
            continue;
        }

        // if(rclick && cell.CheckCollision(mouse) && m_selected != nullptr) {
        //     if (item != nullptr && *item == *m_selected) {
        //         m_selected->sub(item->add(1));
                
        //         if(m_selected->getCount() <= 0) {
        //             m_selected = nullptr;
        //         }
        //         continue;
        //     }

        //     if (item == nullptr) {
        //         auto newItem = std::make_shared<InventorySlot>(*m_selected);
        //         newItem->setCount(1);
        //         m_player->setItem(i, newItem);
        //         m_selected->sub(1);

        //         if(m_selected->getCount() <= 0) {
        //             m_selected = nullptr;
        //         }
        //     }
        // }
    }
}

void InventoryNode::setEnabled(bool val) {
    // if (m_selected != nullptr) {
    //     m_player->addItem(m_selected);
    // }

    Frame::setEnabled(val);
}