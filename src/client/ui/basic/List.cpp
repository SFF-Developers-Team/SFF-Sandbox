#include <ui/basic/List.hpp>
#include <managers/RenderManager.hpp>
#include <managers/TextureManager.hpp>
#include <managers/StyleManager.hpp>
#include <raylib.h>
#include <algorithm>

List::List(std::vector<std::string> const& elements, MiniFunction<void(List*, int)> callback) 
    : Frame(), m_elements(elements), m_callback(callback), m_scrollOffset(0.f), m_selected(0) {}

float List::getTotalHeight() {
    auto elementHeight = StyleManager::get()->getValue<Vec2f>(DEFAULT_ELEMENT_SIZE).y;

    return elementHeight * m_elements.size() + m_border * m_elements.size();
}

float List::getContentHeight() {
    return m_bounds.height - m_border * 2;
}

void List::draw() {
    Frame::draw();

    auto bounds = getWorldBounds();
    auto elementHeight = StyleManager::get()->getValue<Vec2f>(DEFAULT_ELEMENT_SIZE).y;
    auto totalHeight = getTotalHeight();
    auto conHeight = getContentHeight();
    auto borderColor = m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F};
    auto scrollBar = totalHeight > bounds.height;
    auto rect = Rectf {0.f, 0.f, m_bounds.width, conHeight};
    auto selectedColor = m_color;
    selectedColor.brightness(-0.3f);

    Rectf cutList = {
        bounds.x + m_border * getGlobalScaleX(),
        bounds.y + (m_border * 2) * getGlobalScaleY(),
        bounds.width - (m_border * 2) * getGlobalScaleX(),
        conHeight * getGlobalScaleY()
    };

    BeginScissorMode(cutList.x, cutList.y, cutList.width, cutList.height);
        for(auto i = 0; i < m_elements.size(); i++) {
            auto cell = Rectf {
                m_border * 2, 
                m_border * 2 + (elementHeight * i + m_border * i) - m_scrollOffset, 
                m_bounds.width - m_border * (scrollBar ? 5.f : 4.f), 
                elementHeight
            };

            RenderManager::drawFrame(cell, (i == m_selected && m_callback) ? selectedColor : m_color, m_border);
            RenderManager::drawText("boldfont", m_elements[i], {cell.x + cell.width / 2, cell.y + cell.height / 2}, COL_WHITE, 0.f, {0.5f, 0.5f});
        }

        if(scrollBar) {
            RenderManager::drawRect({bounds.width - m_border * 2, rect.y + m_border + (m_scrollOffset / totalHeight) * conHeight, m_border, (conHeight / totalHeight) * conHeight}, borderColor);
        }
    EndScissorMode();
}

void List::update() {
    auto mouse = getLocalMousePosition();
    auto elementHeight = StyleManager::get()->getValue<Vec2f>(DEFAULT_ELEMENT_SIZE).y;
    auto totalHeight = getTotalHeight();

    if(totalHeight > m_bounds.height) {
        if(isMouseHover() && GetMouseWheelMove() != 0.f) {
            m_scrollOffset -= GetMouseWheelMove() * 10.f;
        }

        m_scrollOffset = std::clamp(m_scrollOffset, 0.f, totalHeight - m_bounds.height + m_border * 2);
    }

    for(auto i = 0; i < m_elements.size(); i++) {
        Rectf rect = {
            m_border * 2, 
            m_border * 2 + (elementHeight * i + m_border * i) - m_scrollOffset, 
            m_bounds.width - m_border * 4, 
            elementHeight
        };

        bool isVisible = rect.y + rect.height > m_border && rect.y < m_bounds.height - m_border;

        if(isVisible && rect.contains({mouse.x, mouse.y}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            m_selected = i;
            
            if(m_callback) {
                m_callback(this, i);
            }
        }
    }
}