#include <RaylibExtras.hpp>
#include <raylib-cpp.hpp>
#include <ui/basic/List.hpp>
#include <managers/RenderManager.hpp>
#include <managers/ResourceManager.hpp>
#include <managers/StyleManager.hpp>
#include <algorithm>

List::List(std::vector<std::string> const& elements, MiniFunction<void(List*, int)> callback) 
    : Frame(), m_elements(elements), m_callback(callback), m_scrollOffset(0.f), m_selected(0) {}

float List::getTotalHeight() {
    auto elementHeight = StyleManager::get()->getValue<raylib::Vector2>(DEFAULT_ELEMENT_SIZE).y;

    return elementHeight * m_elements.size() + m_border * m_elements.size();
}

float List::getContentHeight() {
    return m_bounds.height - m_border * 2;
}

void List::draw() {
    Frame::draw();

    auto bounds = getWorldBounds();
    auto elementHeight = StyleManager::get()->getValue<raylib::Vector2>(DEFAULT_ELEMENT_SIZE).y;
    auto totalHeight = getTotalHeight();
    auto conHeight = getContentHeight();
    auto borderColor = m_color.Brightness(-0.5f);
    auto scrollBar = totalHeight > bounds.height;
    auto rect = raylib::Rectangle {0.f, 0.f, m_bounds.width, conHeight};
    auto selectedColor = m_color;
    selectedColor = selectedColor.Brightness(-0.3f);

    raylib::Rectangle cutList = {
        bounds.x + m_border * getGlobalScaleX(),
        bounds.y + (m_border * 2) * getGlobalScaleY(),
        bounds.width - (m_border * 2) * getGlobalScaleX(),
        conHeight * getGlobalScaleY()
    };

    raylib::Scissor scissor({cutList.x, cutList.y, cutList.width, cutList.height});
    {
        for(auto i = 0; i < m_elements.size(); i++) {
            auto cell = raylib::Rectangle {
                m_border * 2, 
                m_border * 2 + (elementHeight * i + m_border * i) - m_scrollOffset, 
                m_bounds.width - m_border * (scrollBar ? 5.f : 4.f), 
                elementHeight
            };

            RenderManager::drawFrame(cell, (i == m_selected && m_callback) ? selectedColor : m_color, m_border);
            auto& font = ResourceManager::Get().GetFont("boldfont");
            float fontSize = static_cast<float>(font.baseSize);
            auto size = font.MeasureText(m_elements[i], fontSize, 1.f);
            font.DrawText(m_elements[i], cell.GetPosition() + (cell.GetSize() - size) / 2.f, fontSize, 1.f, raylib::Color::White());
        }

        if(scrollBar) {
            raylib::Rectangle{bounds.width - m_border * 2, rect.y + m_border + (m_scrollOffset / totalHeight) * conHeight, m_border, (conHeight / totalHeight) * conHeight}.Draw(borderColor);
        }
    }
}

void List::update() {
    auto mouse = getLocalMousePosition();
    auto elementHeight = StyleManager::get()->getValue<raylib::Vector2>(DEFAULT_ELEMENT_SIZE).y;
    auto totalHeight = getTotalHeight();

    if(totalHeight > m_bounds.height) {
        if(isMouseHover() && raylib::Mouse::GetWheelMove() != 0.f) {
            m_scrollOffset -= raylib::Mouse::GetWheelMove() * 10.f;
        }

        m_scrollOffset = std::clamp(m_scrollOffset, 0.f, totalHeight - m_bounds.height + m_border * 2);
    }

    for(auto i = 0; i < m_elements.size(); i++) {
        raylib::Rectangle rect = {
            m_border * 2, 
            m_border * 2 + (elementHeight * i + m_border * i) - m_scrollOffset, 
            m_bounds.width - m_border * 4, 
            elementHeight
        };

        bool isVisible = rect.y + rect.height > m_border && rect.y < m_bounds.height - m_border;

        if(isVisible && rect.CheckCollision(mouse) && raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
            m_selected = i;
            
            if(m_callback) {
                m_callback(this, i);
            }
        }
    }
}