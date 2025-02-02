#include <ui/nodes/List.hpp>
#include <RenderManager.hpp>
#include <raylib.h>
#include <algorithm>

List::List(std::vector<std::string> const& elements, MiniFunction<void(int)> callback) 
    : Frame(), m_elements(elements), m_callback(callback), m_scrollOffset(0.f) {}

void List::draw() {
    Frame::draw();

    auto bounds = getRealBounds();
    auto rm = RenderManager::get();
    auto const totalHeight = 40.f * m_elements.size() + m_border * m_elements.size();
    auto const conHeight = bounds.height - m_border * 2;
    auto const borderColor = m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F};
    auto const scrollBar = totalHeight > bounds.height;

    BeginScissorMode(bounds.x + m_border, bounds.y + m_border, bounds.width - m_border * 2, conHeight);
        for(auto i = 0; i < m_elements.size(); i++) {
            auto rect = Rectf {
                m_border * 2.f, 
                m_border * 2 + 40.f * i + m_border * i - m_scrollOffset, 
                bounds.width - m_border * (scrollBar ? 5.f : 4.f), 
                40.f
            };
            auto size = rm->getTextSize(m_elements[i], "boldfont", 40.f);

            rm->drawRect(rect, m_color);
            rm->drawRectLines(rect, borderColor, m_border);
            rm->drawText("boldfont", m_elements[i], {(rect.width - size.x) / 2, rect.y + (rect.height - size.y) / 2}, COL_WHITE, 40.f);
        }

        if(scrollBar) {
            rm->drawRect({bounds.width - m_border * 2, m_border + (m_scrollOffset / totalHeight) * conHeight, m_border, (conHeight / totalHeight) * conHeight}, borderColor);
        }
    EndScissorMode();
}

void List::update() {
    auto bounds = getRealBounds();
    auto mouse = GetMousePosition();
    auto totalHeight = m_border * 2 + 40.f * m_elements.size() + m_border * m_elements.size();

    if(totalHeight > m_bounds.height) {
        if(bounds.contains({mouse.x, mouse.y}) && GetMouseWheelMove() != 0.f) {
            m_scrollOffset -= GetMouseWheelMove() * 10.f;
        }

        m_scrollOffset = std::clamp(m_scrollOffset, 0.f, totalHeight - bounds.height + m_border);
    }

    for(auto i = 0; i < m_elements.size(); i++) {
        auto rect = Rectf {bounds.x + m_border * 2, bounds.y + m_border * 2 + 40.f * i + m_border * i - m_scrollOffset, bounds.width - m_border * 4, 40.f};
        auto const isVisible = rect.y + rect.height > bounds.y + m_border && rect.y < bounds.y + bounds.height - m_border;

        if(isVisible && rect.contains({mouse.x, mouse.y}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            return m_callback(i);
        }
    }
}