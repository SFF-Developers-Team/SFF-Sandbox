#include <ui/nodes/Inventory.hpp>
#include <RenderManager.hpp>
#include <raylib.h>

float constexpr cellSize = 32.f;
float constexpr blockSize = cellSize / 2.f;
int const columnCount = 9;
int const rowCount = 4;

Inventory::Inventory(std::vector<std::shared_ptr<Block>>& inventory) : Frame(), m_inventory(inventory), m_selected(-1) {
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
        Rectf cell = getCellPosition(i);
        Rectf block = {cell.x + (cell.width - blockSize) / 2, cell.y + (cell.height - blockSize) / 2, blockSize, blockSize};

        RenderManager::drawFrame(cell, m_color, m_border);
            
        if(m_inventory[i] != nullptr && i != m_selected) {
            RenderManager::renderBlock(block, m_inventory[i]);
        }
    }

    if(m_selected >= 0) {
        auto mouse = getLocalMousePosition();

        Rectf block = {mouse.x - blockSize / 2, mouse.y - blockSize / 2, blockSize, blockSize};
        RenderManager::renderBlock(block, m_inventory[m_selected]);
    }
}

void Inventory::update() {
    auto mouse = getLocalMousePosition();

    for (auto i = 0; i < columnCount * rowCount; i++) {
        auto cell = getCellPosition(i);
        auto click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        if(click && cell.contains(mouse) && m_selected < 0) {
            m_selected = i;

            continue;
        }

        if(click && cell.contains(mouse) && m_selected >= 0) {
            if (m_inventory[i] == nullptr) {
                m_inventory[i] = m_inventory[m_selected];
                m_inventory[m_selected] = nullptr;
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