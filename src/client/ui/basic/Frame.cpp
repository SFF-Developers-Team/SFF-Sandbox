#include <ui/basic/Frame.hpp>
#include <StyleManager.hpp>
#include <RenderManager.hpp>

Frame::Frame() : Node() {
    auto sm = StyleManager::get();
    
    setBorderWidth(sm->getValue<float>(DEFAULT_BORDER_WIDTH));
    setColor(sm->getValue<Col4u>(DEFAULT_UI_COLOR));
    setSize(StyleManager::get()->getValue<Vec2f>(DEFAULT_ELEMENT_SIZE));
}

void Frame::draw() {
    Node::draw();
    RenderManager::drawFrame({0.f, 0.f, getWidth(), getHeight()}, m_color, m_border);
}