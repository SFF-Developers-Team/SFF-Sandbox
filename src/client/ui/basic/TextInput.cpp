#include <RaylibExtras.hpp>
#include <raylib-cpp.hpp>
#include <ui/basic/TextInput.hpp>
#include <managers/StyleManager.hpp>
#include <managers/ResourceManager.hpp>
#include <managers/RenderManager.hpp>
#include <Logger.hpp>

TextInput::TextInput(std::string const& font, std::string const& placeholder) : Frame(), m_font(font), m_placeholder(placeholder), m_allowedChars(DEFAULT_ALLOWED_CHARS) {}

void TextInput::draw() {
    Frame::draw();

    auto sm = StyleManager::get();
    auto& tm = ResourceManager::Get();
    auto& font = tm.GetFont(m_font);

    auto fontsize = tm.GetFontBaseSize("font");
    auto textsize = font.MeasureText(m_text, fontsize, 1.f);
    auto cursorpos = font.MeasureText(m_text.substr(0, m_cursorX), fontsize, 1.f).x;
    auto mouse = raylib::Mouse::GetPosition();
    auto color = raylib::Color::White();

    if(m_text.empty()) color.a = 0x7F;

    auto textX = m_border + 4.f;
    if(cursorpos > getWidth() - m_border * 2) {
        textX -= cursorpos - getWidth() + m_border * 2.f + 8.f; 
    }

    auto bounds = getWorldBounds();
    raylib::Scissor scissor({bounds.x + m_border, bounds.y + m_border, bounds.width - m_border * 2.f, bounds.height - m_border * 2.f});
    {
        auto& font = tm.GetFont(m_font);
        auto textY = (getHeight() - font.MeasureText("Ag", fontsize, 1.f).y) / 2.f;
        font.DrawText((m_text.empty() ? m_placeholder : m_text), {textX, textY}, fontsize, 1.f, color);
        
        if(m_active) {
            raylib::Rectangle(textX + cursorpos, (getHeight() - fontsize) / 2, 1.f, fontsize).Draw(raylib::Color::White());
        }
    }
}

void TextInput::update() {
    auto mouse = raylib::Mouse::GetPosition();
    auto sm = StyleManager::get();
    auto cursorhover = getWorldBounds().CheckCollision(mouse);
    m_color = sm->getValue<raylib::Color>(DEFAULT_UI_COLOR);

    if(raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
        m_active = cursorhover;
    }

    if(cursorhover && !m_active) {
        m_color = m_color.Brightness(-0.3f);
    }

    if(m_active) {
        m_color = m_color.Brightness(-0.6f);
        auto isKeyHold = [](int key) -> bool { return raylib::Keyboard::IsKeyPressed(key) || raylib::Keyboard::IsKeyPressedRepeat(key); };
        char c = raylib::Keyboard::GetCharPressed();

        if(m_allowedChars.find(c) != std::string::npos && m_text.size() < m_maxChars) {
            m_text.insert(m_text.begin() + m_cursorX, c);
            m_cursorX++;
        }

        if(raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL) && isKeyHold(KEY_V)) {
            auto text = raylib::Window::GetClipboardText();
            m_text.append(text);
            m_cursorX += text.size();
        }

        if(raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL) && isKeyHold(KEY_BACKSPACE)) {
            auto it = m_text.find_last_of(" ", m_cursorX) + 1;
            m_text.erase(it, m_cursorX);
            m_cursorX -= m_cursorX - it;
        }


        if(isKeyHold(KEY_BACKSPACE) && m_cursorX > 0) {
            m_text.erase(m_cursorX - 1, 1);
            m_cursorX--;
        }

        if(isKeyHold(KEY_DELETE) && m_cursorX < m_text.size()) {
            m_text.erase(m_cursorX, 1);
        }

        if(raylib::Keyboard::IsKeyPressed(KEY_HOME) || raylib::Keyboard::IsKeyPressed(KEY_UP)) {
            m_cursorX = 0;
        }

        if(raylib::Keyboard::IsKeyPressed(KEY_END) || raylib::Keyboard::IsKeyPressed(KEY_DOWN) || raylib::Keyboard::IsKeyPressed(KEY_PAGE_DOWN)) {
            m_cursorX = m_text.size();
        }

        if(isKeyHold(KEY_LEFT) && m_cursorX > 0) {
            m_cursorX--;
        }

        if(isKeyHold(KEY_RIGHT) && m_cursorX < m_text.size()) {
            m_cursorX++;
        }
    }
}

std::string const& TextInput::getText() {
    return m_text;
}

int TextInput::getLength() {
    return m_text.length();
}

void TextInput::setAllowedChars(std::string const& allowedChars) {
    m_allowedChars = allowedChars;
}