#include <ui/nodes/Frame.hpp>
#include <StyleManager.hpp>
#include <RenderManager.hpp>

Frame::Frame() : Node() {
    auto sm = StyleManager::get();
    m_border = sm->getValue<float>(BORDER_WIDTH);
    m_color = sm->getValue<Col4u>(FIRST_COLOR_NORMAL);
}

void Frame::draw() {
    auto rm = RenderManager::get();
    auto rect = Rectf {0.f, 0.f, m_bounds.width, m_bounds.height};

    rm->drawRect(rect, m_color);
    rm->drawRectLines(rect, m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F}, m_border);
}