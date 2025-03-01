#include <ui/basic/DropDown.hpp>
#include <RenderManager.hpp>
#include <TextureManager.hpp>
#include <StyleManager.hpp>
#include <algorithm>
#include <raylib.h>
#include <format>
#include <rlgl.h>

DropDown::DropDown(std::vector<std::string> const& elements, MiniFunction<void(List*, int)> callback) : 
    List(elements, callback), m_opened(false), m_maxHeight(100.f), m_mask("{}") {}

float DropDown::getContentHeight() {
    return std::min(getTotalHeight() + m_border * 3, m_maxHeight + m_border * 2);
}

void DropDown::draw() {
    Frame::draw();

    auto bounds = getWorldBounds();
    auto selected = std::vformat(m_mask, std::make_format_args(m_elements[m_selected]));
    auto size = RenderManager::getTextSize(selected, "boldfont");
    auto arrowSize = RenderManager::getTextSize("^", "boldfont");

    RenderManager::drawText("boldfont", selected, {m_bounds.width / 2, m_bounds.height / 2}, COL_WHITE, 0.f, {0.5f, 0.5f});
    rlPushMatrix();
        rlTranslatef(m_bounds.width - m_border - m_bounds.height / 2, m_bounds.height / 2, 0);
        rlRotatef((!m_opened ? 180 : 0), 0, 0, 1);

        RenderManager::drawText("boldfont", "^", {-arrowSize.x / 2, -arrowSize.y / 3}, COL_WHITE);
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

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_opened = (isMouseHover()) ? !m_opened : false;
    }
}