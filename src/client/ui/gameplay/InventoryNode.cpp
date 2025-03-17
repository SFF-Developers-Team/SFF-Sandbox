#include <ui/gameplay/InventoryNode.hpp>
#include <managers/RenderManager.hpp>
#include <raylib.h>

float constexpr cellSize = 24.f;
float constexpr blockSize = 16.f;
int const columnCount = 9;
int const rowCount = 4;

InventoryNode::InventoryNode(std::shared_ptr<LocalPlayer> player) : Frame(), m_player(player) {
    setSize({
        cellSize * columnCount + m_border * (columnCount + 3), 
        cellSize * rowCount + m_border * (rowCount + 4)
    });
}

Rectf InventoryNode::getCellPosition(int i) {
    auto cellX = i % columnCount;
    auto cellY = i / columnCount;

    Rectf cell = {
        m_border * 2 + cellSize * cellX + m_border * cellX, 
        m_border * 2 + cellSize * cellY + m_border * cellY + (cellY > 0 ? m_border : 0), 
        cellSize, cellSize
    };

    return cell;
}

void InventoryNode::draw() {
    Frame::draw();

    for (auto i = 0; i < m_player->getSlotsCount(); i++) {
        Rectf cellRect = getCellPosition(i);
        Rectf itemRect = {cellRect.x + (cellRect.width - blockSize) / 2, cellRect.y + (cellRect.height - blockSize) / 2, blockSize, blockSize};
        Vec2f mouse = getLocalMousePosition();

        RenderManager::drawFrame(cellRect, m_color, m_border);
            
        if (m_player->getItem(i) != nullptr) {
            RenderManager::renderInventoryItem(itemRect, m_player->getItem(i));
        }

        if (cellRect.contains(mouse)) {
            RenderManager::drawRect(cellRect, {255, 255, 255, 64});
        }
    }

    if(m_selected != nullptr) {
        auto mouse = getLocalMousePosition();

        Rectf blockRect = {mouse.x - blockSize / 2, mouse.y - blockSize / 2, blockSize, blockSize};
        RenderManager::renderInventoryItem(blockRect, m_selected);
    }
}

void InventoryNode::update() {
    auto mouse = getLocalMousePosition();

    for (auto i = 0; i < columnCount * rowCount; i++) {
        auto cell = getCellPosition(i);
        auto lclick = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        auto rclick = IsMouseButtonPressed(MOUSE_RIGHT_BUTTON);
        auto item = m_player->getItem(i);
        auto& inv = m_player->getInventory();

        if(lclick && cell.contains(mouse) && m_selected == nullptr && item != nullptr) {
            m_selected = std::move(inv[i]);

            continue;
        }

        if(lclick && cell.contains(mouse) && m_selected != nullptr) {
            if (item != nullptr) {
                if (*item == *m_selected) {
                    m_selected->sub(item->add(m_selected->getCount()));
                    
                    if(m_selected->getCount() <= 0) {
                        m_selected = nullptr;
                    }
                    continue;
                }

                std::swap(m_selected, inv[i]);
                continue;
            }

            m_player->setItem(i, std::move(m_selected));
        }

        if(rclick && cell.contains(mouse) && m_selected != nullptr) {
            if (item != nullptr && *item == *m_selected) {
                m_selected->sub(item->add(1));
                
                if(m_selected->getCount() <= 0) {
                    m_selected = nullptr;
                }
                continue;
            }

            if (item == nullptr) {
                auto newItem = std::make_shared<InventoryItem>(*m_selected);
                newItem->setCount(1);
                m_player->setItem(i, newItem);
                m_selected->sub(1);

                if(m_selected->getCount() <= 0) {
                    m_selected = nullptr;
                }
            }
        }

        if(rclick && cell.contains(mouse) && m_selected == nullptr && item != nullptr) {
            if (item->getCount() == 1) {
                m_selected = std::move(inv[i]);
                continue;
            }

            int count = item->getCount() / 2;

            m_selected = std::make_shared<InventoryItem>(*item);
            m_selected->setCount(count);
            item->sub(count);
        }
    }
}

void InventoryNode::setEnabled(bool val) {
    if (m_selected != nullptr) {
        m_player->addItem(m_selected);
    }

    Frame::setEnabled(val);
}