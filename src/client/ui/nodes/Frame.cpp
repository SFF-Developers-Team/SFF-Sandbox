#include <ui/nodes/Frame.hpp>
#include <StyleManager.hpp>
#include <RenderManager.hpp>

Frame::Frame() : Node() {
    auto sm = StyleManager::get();
    
    setBorderWidth(sm->getValue<float>(DEFAULT_BORDER_WIDTH));
    setColor(sm->getValue<Col4u>(DEFAULT_UI_COLOR));
    setSize(StyleManager::get()->getValue<Vec2f>(DEFAULT_ELEMENT_SIZE));
}

void Frame::draw() {
    auto rm = RenderManager::get();
    auto rect = Rectf {0.f, 0.f, m_bounds.width, m_bounds.height};

    rm->drawRect(rect, m_color);
    
    if(m_border > 0) {
        rm->drawRectLines(rect, m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F}, m_border);
    }
}

void Frame::setBorderWidth(float width) {
    m_border = width;
}

float Frame::getBorderWidth() {
    return m_border;
}