#include <ui/nodes/DropDown.hpp>
#include <RenderManager.hpp>
#include <raylib.h>
#include <rlgl.h>
#include <Logger.hpp>

DropDown::DropDown(std::vector<std::string> const& elements, MiniFunction<void(DropDown*, int)> callback) : 
    Frame(), m_elements(elements), m_callback(callback), m_selected(0), m_opened(false), m_maxHeight(300.f) {}

void DropDown::draw() {
    Frame::draw();

    auto rm = RenderManager::get();
    auto size = rm->getTextSize(m_elements[m_selected], "boldfont", 40.f);
    auto arrowsize = rm->getTextSize("^", "boldfont", 40.f);
    auto textpos = Vec2f {(m_bounds.width - size.x) / 2 - m_bounds.height, (m_bounds.height - size.y) / 2}; 

    rm->drawText("boldfont", m_elements[m_selected], textpos, COL_WHITE, 40.f);
    rlPushMatrix();
        rlTranslatef(m_bounds.width - m_border - m_bounds.height / 2, m_bounds.height / 2, 0);
        rlRotatef(180, 0, 0, 1);
        rm->drawText("boldfont", "^", {-arrowsize.x / 2, -arrowsize.y / 3}, COL_WHITE, 40.f);
    rlPopMatrix();

    if(m_opened) {
        rm->drawRect({0.f, m_bounds.height, m_bounds.width, m_maxHeight}, COL_RED);
    }
}

void DropDown::update() {
    Frame::update();
    auto bounds = getRealBounds();
    auto mouse = GetMousePosition();

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_opened = bounds.contains({mouse.x, mouse.y});
        logD("Opened {}", m_opened);
    }
}

void DropDown::setMaxHeight(float height) {
    m_maxHeight = height;
}

float DropDown::getMaxHeight() {
    return m_maxHeight;
}