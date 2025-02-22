#include <ui/nodes/ScrollableContainer.hpp>
#include <RenderManager.hpp>
#include <algorithm>
#include <raylib.h>
#include <rlgl.h>

ScrollableContainer::ScrollableContainer() : Container() {}

float ScrollableContainer::calculateTotalHeight() {
    float ret = 0.f;
    
    for(auto& node : m_childs) {
        ret = std::max(ret, node->getY() + node->getHeight());
    }

    return ret;
}

void ScrollableContainer::draw() {
    auto const bounds = getWorldBounds();
    auto const totalHeight = calculateTotalHeight();
    auto const contentHeight = m_bounds.height - m_border * 2;
    auto const borderColor = m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F};
    auto const scrollBar = totalHeight > bounds.height;

    Rectf const cutRect = {
        bounds.x + m_border * getGlobalScaleX(),
        bounds.y + m_border * getGlobalScaleY(),
        bounds.width - (m_border * 2) * getGlobalScaleX(),
        contentHeight * getGlobalScaleY()
    };


    BeginScissorMode(cutRect.x, cutRect.y, cutRect.width, cutRect.height);
        Container::draw();

        if(scrollBar) {
            RenderManager::drawRect({m_bounds.width - m_border * 2, m_border + (m_offset.y / totalHeight) * contentHeight, m_border, (contentHeight / totalHeight) * contentHeight}, borderColor);
        }
    EndScissorMode();
}

void ScrollableContainer::update() {
    Container::update();

    auto const totalHeight = calculateTotalHeight();
    auto const contentHeight = (m_bounds.height - m_border * 2);

    if(totalHeight > m_bounds.height) {
        if(isMouseHover() && GetMouseWheelMove() != 0.f) {
            m_offset.y -= GetMouseWheelMove() * 10.f;
        }

        m_offset.y = std::clamp(m_offset.y, 0.f, totalHeight - contentHeight);
    }
}