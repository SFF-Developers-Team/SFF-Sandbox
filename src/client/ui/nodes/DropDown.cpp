#include <ui/nodes/DropDown.hpp>
#include <RenderManager.hpp>
#include <raylib.h>
#include <rlgl.h>
#include <Logger.hpp>
#include <algorithm>

DropDown::DropDown(std::vector<std::string> const& elements, MiniFunction<void(DropDown*, int)> callback) : 
    Frame(), m_elements(elements), m_callback(callback), m_selected(0), m_opened(false), m_maxHeight(200.f), m_scrollOffset(0.f), m_mask("{}") {}

void DropDown::draw() {
    Frame::draw();

    auto rm = RenderManager::get();
    auto selected = std::vformat(m_mask, std::make_format_args(m_elements[m_selected]));
    auto size = rm->getTextSize(selected, "boldfont", 40.f);
    auto arrowsize = rm->getTextSize("^", "boldfont", 40.f);
    auto textpos = Vec2f {(m_bounds.width - size.x) / 2, (m_bounds.height - size.y) / 2};
    auto bounds = getWorldBounds();

    rm->drawText("boldfont", selected, textpos, COL_WHITE, 40.f);
    rlPushMatrix();
        rlTranslatef(m_bounds.width - m_border - m_bounds.height / 2, m_bounds.height / 2, 0);
        rlRotatef((!m_opened ? 180 : 0), 0, 0, 1);
        rm->drawText("boldfont", "^", {-arrowsize.x / 2, -arrowsize.y / 3}, COL_WHITE, 40.f);
    rlPopMatrix();

    if(m_opened) {
        auto const totalHeight = 40.f * m_elements.size() + m_border * m_elements.size();
        auto const conHeight = std::min(totalHeight, m_maxHeight);
        auto const borderColor = m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F};
        auto const scrollBar = totalHeight > m_maxHeight;
        auto const rect = Rectf {0.f, m_bounds.height + m_border, m_bounds.width, conHeight};

        rm->drawRect(rect, m_color);
        rm->drawRectLines(rect, m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F}, m_border);

        BeginScissorMode(bounds.x + m_border, bounds.y + bounds.height + m_border * 2, rect.width, conHeight - m_border * 2);
            for(auto i = 0; i < m_elements.size(); i++) {
                auto cell = Rectf {
                    m_border * 2.f, 
                    rect.y + m_border * 2.f + 40.f * i + m_border * i - m_scrollOffset, 
                    bounds.width - m_border * (scrollBar ? 5.f : 4.f), 
                    40.f
                };
                auto size = rm->getTextSize(m_elements[i], "boldfont", 40.f);

                rm->drawRect(cell, m_color);
                rm->drawRectLines(cell, borderColor, m_border);
                rm->drawText("boldfont", m_elements[i], {(cell.width - size.x) / 2, cell.y + (cell.height - size.y) / 2}, COL_WHITE, 40.f);
            }

            if(scrollBar) {
                rm->drawRect({bounds.width - m_border * 2, rect.y + m_border + (m_scrollOffset / totalHeight) * conHeight, m_border, (conHeight / totalHeight) * conHeight}, borderColor);
            }
        EndScissorMode();
    }
}

void DropDown::update() {
    Frame::update();
    auto bounds = getWorldBounds();
    auto mouse = GetMousePosition();

    if(m_opened && getZOrder() < 100) {
        setZOrder(100);
    }

    if(!m_opened && getZOrder() > 0) {
        setZOrder(-1);
    }

    if(m_opened) {
        auto const totalHeight = m_border * 4 + 40.f * m_elements.size() + m_border * m_elements.size();
        auto const listBounds = Rectf {bounds.x, bounds.y + bounds.height + m_border, bounds.width, std::min(totalHeight, m_maxHeight)};

        if(totalHeight > m_maxHeight) {
            if(listBounds.contains({mouse.x, mouse.y}) && GetMouseWheelMove() != 0.f) {
                m_scrollOffset -= GetMouseWheelMove() * 30.f;
            }

            m_scrollOffset = std::clamp(m_scrollOffset, 0.f, totalHeight - listBounds.height + m_border);
        }

        for(auto i = 0; i < m_elements.size(); i++) {
            auto rect = Rectf {listBounds.x + m_border * 2, listBounds.y + m_border * 2 + 40.f * i + m_border * i - m_scrollOffset, listBounds.width - m_border * 4, 40.f};
            auto const isVisible = rect.y + rect.height > listBounds.y + m_border && rect.y < listBounds.y + listBounds.height - m_border;

            if(isVisible && rect.contains({mouse.x, mouse.y}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                m_selected = i;
                m_opened = false;

                return m_callback(this, i);
            }
        }
    }

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_opened = (bounds.contains({mouse.x, mouse.y}) ? !m_opened : false);
    }
}

void DropDown::setMaxHeight(float height) {
    m_maxHeight = height;
}

float DropDown::getMaxHeight() {
    return m_maxHeight;
}

void DropDown::setMask(std::string const& mask) {
    m_mask = mask;
}

std::string const& DropDown::getMask() {
    return m_mask;
}