#include "Rectangle.hpp"

void sandbox_ui::Rectangle::draw() {
    auto rect = getRenderableRectangle();

    if (m_rectMode == Filled) {
        DrawRectanglePro(rect, {0, 0}, 0.f, getColor());
    } else if (m_rectMode == Outlined) {
        DrawRectangleLinesEx(rect, m_thickness, getColor());
    }
}

void sandbox_ui::Rectangle::setThickness(float v) {
    m_thickness = v;
}
void sandbox_ui::Rectangle::setMode(Mode mode) {
    m_rectMode = mode;
}

bool sandbox_ui::Rectangle::collides(sandbox_ui::Rectangle *ui_rect) {
    return collides(ui_rect->getRenderableRectangle());
}
bool sandbox_ui::Rectangle::collides(Pos point) {
    return CheckCollisionPointRec(point, getRenderableRectangle());
}
bool sandbox_ui::Rectangle::collides(sandbox_ui::Node::Rect rl_rect) {
    return CheckCollisionRecs(getRenderableRectangle(), rl_rect);
}

sandbox_ui::Node::Rect sandbox_ui::Rectangle::getRenderableRectangle() {
    auto rect1 = getRectangle();
    rect1.width *= getScale();
    rect1.height *= getScale();

    return rect1;
}

sandbox_ui::Rectangle::Rectangle() {}
sandbox_ui::Rectangle::Rectangle(Node::Rect r) {
    setPosition({ r.x, r.y });
    setSize({ r.width, r.height });
}