#include <RaylibExtras.hpp>
#include <raylib-cpp.hpp>
#include <ui/basic/ScrollableContainer.hpp>
#include <managers/RenderManager.hpp>
#include <algorithm>
#include <rlgl.h>

ScrollableContainer::ScrollableContainer() : Container() {}

raylib::Vector2 ScrollableContainer::getTotalSize() {
    raylib::Vector2 ret;
    
    for(auto& node : m_childs) {
        ret.x = std::max(ret.x, node->getRightX());
        ret.y = std::max(ret.y, node->getBottomY());
    }

    return ret;
}

void ScrollableContainer::draw() {
    auto bounds = getWorldBounds();
    raylib::Vector2 totalSize = getTotalSize();
    raylib::Vector2 contentSize = {m_bounds.width - m_border * 2, m_bounds.height - m_border * 2};
    raylib::Color color = m_color;
    color = color.Brightness(-0.25f);

    raylib::Rectangle cutRect = {
        bounds.x + m_border * getGlobalScaleX(),
        bounds.y + m_border * getGlobalScaleY(),
        bounds.width - (m_border * 2) * getGlobalScaleX(),
        contentSize.y * getGlobalScaleY()
    };

    Frame::draw();

    raylib::Scissor scissor({cutRect.x, cutRect.y, cutRect.width, cutRect.height});
    {
        Container::draw();

        if (totalSize.y > bounds.height) {
            raylib::Rectangle{
                m_bounds.width - m_border * 2, 
                m_border + (m_offset.y / totalSize.y) * contentSize.y, m_border, 
                (contentSize.y / totalSize.y) * contentSize.y}.Draw(color);
        }

        if (totalSize.x > bounds.width) {
            raylib::Rectangle{
                m_border + (m_offset.x / totalSize.x) * contentSize.x,
                m_bounds.height - m_border * 2,
                (contentSize.x / totalSize.x) * contentSize.x, m_border
            }.Draw(color);
        }
    }
}

void ScrollableContainer::update() {
    Container::update();

    raylib::Vector2 totalSize = getTotalSize();
    raylib::Vector2 contentSize = {m_bounds.width - m_border * 2, m_bounds.height - m_border * 2};

    if(isMouseHover() && raylib::Mouse::GetWheelMove() != 0.f) {
        if(totalSize.x > m_bounds.width && raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL)) {
            m_offset.x -= raylib::Mouse::GetWheelMove() * 10.f;
            m_offset.x = std::clamp(m_offset.x, 0.f, totalSize.x - contentSize.x);
        }

        if(totalSize.y > m_bounds.height && !raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL)) {
            m_offset.y -= raylib::Mouse::GetWheelMove() * 10.f;
            m_offset.y = std::clamp(m_offset.y, 0.f, totalSize.y - contentSize.y);
        }
    }
}