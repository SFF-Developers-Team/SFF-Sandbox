#include <ui/nodes/Inventory.hpp>
#include <RenderManager.hpp>
#include <raylib.h>

float constexpr cellSize = 32.f;
float constexpr blockSize = cellSize / 2.f;
int const columnCount = 9;
int const rowCount = 4;

Inventory::Inventory(std::vector<InventoryItem>& inventory) : Frame(), m_inventory(inventory), m_selected(-1) {
    setSize({
        cellSize * columnCount + m_border * (columnCount + 3), 
        cellSize * rowCount + m_border * (rowCount + 5)
    });
}

Rectf Inventory::getCellPosition(int i) {
    auto cellX = i % columnCount;
    auto cellY = i / columnCount;

    Rectf cell = {
        m_border * 2 + cellSize * cellX + m_border * cellX, 
        m_border * 2 + cellSize * cellY + m_border * cellY + (cellY > 0 ? m_border : 0), 
        cellSize, cellSize
    };

    return cell;
}

void Inventory::draw() {
    Frame::draw();

    for (auto i = 0; i < m_inventory.size(); i++) {
        Rectf cellRect = getCellPosition(i);
        Rectf itemRect = {cellRect.x + (cellRect.width - blockSize) / 2, cellRect.y + (cellRect.height - blockSize) / 2, blockSize, blockSize};

        RenderManager::drawFrame(cellRect, m_color, m_border);
            
        if(m_inventory[i].pointer != nullptr && i != m_selected) {
            RenderManager::renderInventoryItem(itemRect, m_inventory[i]);
            RenderManager::drawText("font", std::to_string(m_inventory[i].count), {cellRect.x + cellRect.width, cellRect.y + cellRect.height}, COL_WHITE, 0.f, {1.f, 1.f});
        }
    }

    if(m_selected >= 0) {
        auto mouse = getLocalMousePosition();

        Rectf blockRect = {mouse.x - blockSize / 2, mouse.y - blockSize / 2, blockSize, blockSize};
        RenderManager::renderInventoryItem(blockRect, m_inventory[m_selected]);
    }
}

void Inventory::update() {
    auto mouse = getLocalMousePosition();

    for (auto i = 0; i < columnCount * rowCount; i++) {
        auto cell = getCellPosition(i);
        auto click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        if(click && cell.contains(mouse) && m_selected < 0 && m_inventory[i].pointer != nullptr) {
            m_selected = i;

            continue;
        }

        if(click && cell.contains(mouse) && m_selected >= 0) {
            if (m_inventory[i].pointer == nullptr) {
                m_inventory[i] = std::move(m_inventory[m_selected]);
            } else {
                std::swap(m_inventory[i], m_inventory[m_selected]);
            }

            m_selected = -1;
        }
    }
}

void Inventory::setEnabled(bool val) {
    m_selected = -1;

    Frame::setEnabled(val);
}