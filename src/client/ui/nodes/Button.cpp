#include <ui/nodes/Button.hpp>
#include <raylib.h>
#include <StyleManager.hpp>

Button::Button(std::string const& text, MiniFunction<void()> const& callback) : Node(), m_text(text), m_callback(callback) {}

void Button::update() {
    auto mouse = GetMousePosition();
    auto bounds = getBoundsAnchor();

    if(bounds.contains({mouse.x, mouse.y}) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        m_callback();
    }
}

void Button::draw() {
    auto sm = StyleManager::get();
    auto fontSize = sm->getValue<float>(TEXT_SIZE);
    auto borderw = sm->getValue<float>(BORDER_WIDTH);
    auto width = MeasureText(m_text.c_str(), fontSize);
    auto mouse = GetMousePosition();
    auto bounds = getBoundsAnchor();

    auto first = FIRST_COLOR_NORMAL;
    auto second = SECOND_COLOR_NORMAL;
    auto textCol = TEXT_COLOR_NORMAL;

    if(bounds.contains({mouse.x, mouse.y}) && IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        first = FIRST_COLOR_FOCUS;
        second = SECOND_COLOR_FOCUS;
        textCol = TEXT_COLOR_FOCUS;
    }

    if(bounds.contains({mouse.x, mouse.y}) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        first = FIRST_COLOR_PRESS;
        second = SECOND_COLOR_PRESS;
        textCol = TEXT_COLOR_PRESS;
    }

    auto bnd = bounds.to<Rectangle>();
    DrawRectangleRec(bnd, sm->getValue<Col4u>(first).to<Color>());
    DrawRectangleLinesEx(bnd, borderw, sm->getValue<Col4u>(second).to<Color>());
    DrawText(m_text.c_str(), bnd.x + bnd.width / 2 - width / 2, bnd.y + bnd.height / 2 - fontSize / 2, fontSize, sm->getValue<Col4u>(textCol).to<Color>());
}