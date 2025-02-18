#include <ui/nodes/List.hpp>
#include <RenderManager.hpp>
#include <TextureManager.hpp>
#include <raylib.h>
#include <algorithm>

List::List(std::vector<std::string> const& elements, MiniFunction<void(List*, int)> callback) 
    : Frame(), m_elements(elements), m_callback(callback), m_scrollOffset(0.f) {}

void List::draw() {
    Frame::draw();

    auto bounds = getWorldBounds();
    auto const fontSize = TextureManager::get()->getFontBaseSize("boldfont");
    auto const totalHeight = fontSize * m_elements.size() + m_border * m_elements.size();
    auto const conHeight = bounds.height - m_border * 2;
    auto const borderColor = m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F};
    auto const scrollBar = totalHeight > bounds.height;
    auto const rect = Rectf {0.f, 0.f, m_bounds.width, conHeight};

    Rectf cutList = {
        bounds.x + m_border * getGlobalScaleX(),
        bounds.y + (m_border * 2) * getGlobalScaleY(),
        bounds.width - (m_border * 2) * getGlobalScaleX(),
        (conHeight - m_border * 2) * getGlobalScaleY()
    };

    BeginScissorMode(cutList.x, cutList.y, cutList.width, cutList.height);
        for(auto i = 0; i < m_elements.size(); i++) {
            auto cell = Rectf {
                m_border * 2.f, 
                rect.y + m_border * 2.f + fontSize * i + m_border * i - m_scrollOffset, 
                m_bounds.width - m_border * (scrollBar ? 5.f : 4.f), 
                fontSize
            };

            RenderManager::drawRect(cell, m_color);
            RenderManager::drawRectLines(cell, borderColor, m_border);
            RenderManager::drawText("boldfont", m_elements[i], {cell.width / 2, cell.y + cell.height / 2}, COL_WHITE, fontSize, {0.5f, 0.5f});
        }

        if(scrollBar) {
            RenderManager::drawRect({bounds.width - m_border * 2, rect.y + m_border + (m_scrollOffset / totalHeight) * conHeight, m_border, (conHeight / totalHeight) * conHeight}, borderColor);
        }
    EndScissorMode();
}

void List::update() {
    auto bounds = getWorldBounds();
    auto mouse = GetMousePosition();
    auto fontSize = TextureManager::get()->getFontBaseSize("boldfont");
    auto totalHeight = m_border * 2 + fontSize * m_elements.size() + m_border * m_elements.size();

    if(totalHeight > bounds.height) {
        if(bounds.contains({mouse.x, mouse.y}) && GetMouseWheelMove() != 0.f) {
            m_scrollOffset -= GetMouseWheelMove() * 10.f;
        }

        m_scrollOffset = std::clamp(m_scrollOffset, 0.f, totalHeight - bounds.height + m_border);
    }

    for(auto i = 0; i < m_elements.size(); i++) {
        auto rect = Rectf {bounds.x + m_border * 2, bounds.y + m_border * 2 + fontSize * i + m_border * i - m_scrollOffset, bounds.width - m_border * 4, fontSize};
        auto const isVisible = rect.y + rect.height > bounds.y + m_border && rect.y < bounds.y + bounds.height - m_border;

        if(isVisible && rect.contains({mouse.x, mouse.y}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            return m_callback(this, i);
        }
    }
}