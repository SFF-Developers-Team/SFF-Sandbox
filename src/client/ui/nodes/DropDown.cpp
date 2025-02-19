#include <ui/nodes/DropDown.hpp>
#include <RenderManager.hpp>
#include <TextureManager.hpp>
#include <algorithm>
#include <raylib.h>
#include <format>
#include <rlgl.h>

DropDown::DropDown(std::vector<std::string> const& elements, MiniFunction<void(DropDown*, int)> callback) : 
    Frame(), m_elements(elements), m_callback(callback), m_selected(0), m_opened(false), m_maxHeight(100.f), m_scrollOffset(0.f), m_mask("{}") {}

void DropDown::draw() {
    Frame::draw();

    auto bounds = getWorldBounds();
    auto fontsize = TextureManager::get()->getFontBaseSize("boldfont");
    auto selected = std::vformat(m_mask, std::make_format_args(m_elements[m_selected]));
    auto size = RenderManager::getTextSize(selected, "boldfont", fontsize);
    auto arrowSize = RenderManager::getTextSize("^", "boldfont", fontsize);

    RenderManager::drawText("boldfont", selected, {m_bounds.width / 2, m_bounds.height / 2}, COL_WHITE, fontsize, {0.5f, 0.5f});
    rlPushMatrix();
        rlTranslatef(m_bounds.width - m_border - m_bounds.height / 2, m_bounds.height / 2, 0);
        rlRotatef((!m_opened ? 180 : 0), 0, 0, 1);

        RenderManager::drawText("boldfont", "^", {-arrowSize.x / 2, -arrowSize.y / 3}, COL_WHITE, fontsize);
    rlPopMatrix();

    if(m_opened) {
        auto const totalHeight = fontsize * m_elements.size() + m_border * m_elements.size();
        auto const conHeight = std::min(totalHeight, m_maxHeight);
        auto const borderColor = m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F};
        auto const scrollBar = totalHeight > m_maxHeight;
        auto const rect = Rectf {0.f, m_bounds.height + m_border, m_bounds.width, conHeight};

        RenderManager::drawFrame(rect, m_color, m_border);

        Rectf cutList = {
            bounds.x + m_border * getGlobalScaleX(),
            bounds.y + bounds.height + m_border * 2 * getGlobalScaleY(),
            bounds.width - m_border * 2 * getGlobalScaleX(),
            (conHeight - m_border * 2) * getGlobalScaleY()
        };

        BeginScissorMode(cutList.x, cutList.y, cutList.width, cutList.height);
            for(auto i = 0; i < m_elements.size(); i++) {
                auto cell = Rectf {
                    m_border * 2.f, 
                    rect.y + m_border * 2.f + fontsize * i + m_border * i - m_scrollOffset, 
                    m_bounds.width - m_border * (scrollBar ? 5.f : 4.f), 
                    fontsize
                };

                RenderManager::drawFrame(cell, m_color, m_border);
                RenderManager::drawText("boldfont", m_elements[i], {cell.width / 2, cell.y + cell.height / 2}, COL_WHITE, fontsize, {0.5f, 0.5f});
            }

            if(scrollBar) {
                RenderManager::drawRect({m_bounds.width - m_border * 2, rect.y + m_border + (m_scrollOffset / totalHeight) * conHeight, m_border, (conHeight / totalHeight) * conHeight}, borderColor);
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
        auto const totalHeight = m_border * 4 + 20.f * m_elements.size() + m_border * m_elements.size();
        
        auto const listBounds = Rectf {
            bounds.x, 
            bounds.y + bounds.height + m_border, 
            bounds.width, 
            std::min(totalHeight, m_maxHeight)
        };

        if(totalHeight > m_maxHeight) {
            if(listBounds.contains({mouse.x, mouse.y}) && GetMouseWheelMove() != 0.f) {
                m_scrollOffset -= GetMouseWheelMove() * 30.f;
            }

            m_scrollOffset = std::clamp(m_scrollOffset, 0.f, totalHeight - listBounds.height - m_border);
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