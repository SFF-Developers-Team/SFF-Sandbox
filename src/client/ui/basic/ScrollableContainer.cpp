#include <ui/basic/ScrollableContainer.hpp>
#include <RenderManager.hpp>
#include <algorithm>
#include <raylib.h>
#include <rlgl.h>

ScrollableContainer::ScrollableContainer() : Container() {}

Vec2f ScrollableContainer::getTotalSize() {
    Vec2f ret;
    
    for(auto& node : m_childs) {
        ret.x = std::max(ret.x, node->getRightX());
        ret.y = std::max(ret.y, node->getBottomY());
    }

    return ret;
}

void ScrollableContainer::draw() {
    Rectf bounds = getWorldBounds();
    Vec2f totalSize = getTotalSize();
    Vec2f contentSize = {m_bounds.width - m_border * 2, m_bounds.height - m_border * 2};
    Col4u color = m_color;
    color.brightness(-0.25f);

    Rectf cutRect = {
        bounds.x + m_border * getGlobalScaleX(),
        bounds.y + m_border * getGlobalScaleY(),
        bounds.width - (m_border * 2) * getGlobalScaleX(),
        contentSize.y * getGlobalScaleY()
    };

    Frame::draw();

    BeginScissorMode(cutRect.x, cutRect.y, cutRect.width, cutRect.height);
        Container::draw();

        if (totalSize.y > bounds.height) {
            RenderManager::drawRect({
                m_bounds.width - m_border * 2, 
                m_border + (m_offset.y / totalSize.y) * contentSize.y, m_border, 
                (contentSize.y / totalSize.y) * contentSize.y}, color
            );
        }

        if (totalSize.x > bounds.width) {
            RenderManager::drawRect({
                m_border + (m_offset.x / totalSize.x) * contentSize.x,
                m_bounds.height - m_border * 2,
                (contentSize.x / totalSize.x) * contentSize.x, m_border
            }, color);
        }
    EndScissorMode();
}

void ScrollableContainer::update() {
    Container::update();

    Vec2f totalSize = getTotalSize();
    Vec2f contentSize = {m_bounds.width - m_border * 2, m_bounds.height - m_border * 2};

    if(isMouseHover() && GetMouseWheelMove() != 0.f) {
        if(totalSize.x > m_bounds.width && IsKeyDown(KEY_LEFT_CONTROL)) {
            m_offset.x -= GetMouseWheelMove() * 10.f;
            m_offset.x = std::clamp(m_offset.x, 0.f, totalSize.x - contentSize.x);
        }

        if(totalSize.y > m_bounds.height && !IsKeyDown(KEY_LEFT_CONTROL)) {
            m_offset.y -= GetMouseWheelMove() * 10.f;
            m_offset.y = std::clamp(m_offset.y, 0.f, totalSize.y - contentSize.y);
        }
    }
}