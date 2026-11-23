#include <raylib-cpp.hpp>
#include <ui/basic/DropDown.hpp>
#include <managers/RenderManager.hpp>
#include <managers/ResourceManager.hpp>
#include <managers/StyleManager.hpp>
#include <algorithm>
#include <format>
#include <rlgl.h>

DropDown::DropDown(std::vector<std::string> const& elements, MiniFunction<void(List*, int)> callback) : 
    List(elements, callback), m_opened(false), m_maxHeight(100.f), m_mask("{}") {}

float DropDown::getContentHeight() {
    return std::min(getTotalHeight() + m_border * 3, m_maxHeight + m_border * 2);
}

void DropDown::draw() {
    Frame::draw();

    auto& rm = ResourceManager::Get();

    auto bounds = getWorldBounds();
    auto selected = std::vformat(m_mask, std::make_format_args(m_elements[m_selected]));
    auto size = rm.GetFont("boldfont").MeasureText(selected, 20.f, 1.f);
    auto arrowSize = rm.GetFont("boldfont").MeasureText("^", 20.f, 1.f);

    rm.GetFont("boldfont").DrawText(selected, {(getWidth() - size.x) / 2.f, (getHeight() - size.y) / 2.f}, 20.f, 1.f, raylib::Color::White());
    rlPushMatrix();
        rlTranslatef(m_bounds.width - m_border - m_bounds.height / 2, m_bounds.height / 2, 0);
        rlRotatef((!m_opened ? 180 : 0), 0, 0, 1);

        rm.GetFont("boldfont").DrawText("^", {-arrowSize.x / 2, -arrowSize.y / 3}, 20.f, 1.f, raylib::Color::White());
    rlPopMatrix();

    if(m_opened) {
        rlTranslatef(0.f, m_bounds.height + m_border, 0.f);
        auto height = m_bounds.height;
        m_bounds.height = getContentHeight();
        
        List::draw();

        m_bounds.height = height;
        rlTranslatef(0.f, -(m_bounds.height + m_border), 0.f);
    }
}

void DropDown::update() {
    Frame::update();
    auto mouse = getLocalMousePosition();

    if(m_opened && getZOrder() < 100) {
        setZOrder(100);
    }

    if(!m_opened && getZOrder() > 0) {
        setZOrder(-1);
    }

    if(m_opened) {
        rlTranslatef(0.f, m_bounds.height + m_border, 0.f);
        auto height = m_bounds.height;
        m_bounds.height = getContentHeight();
        
        List::update();

        m_bounds.height = height;
        rlTranslatef(0.f, -(m_bounds.height + m_border), 0.f);
    }

    if(raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_opened = (isMouseHover()) ? !m_opened : false;
    }
}