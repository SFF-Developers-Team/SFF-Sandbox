#include <ui/nodes/TextInput.hpp>
#include <StyleManager.hpp>
#include <TextureManager.hpp>
#include <RenderManager.hpp>
#include <raylib.h>
#include <Logger.hpp>

TextInput::TextInput(std::string const& font, std::string const& placeholder) : Frame(), m_font(font), m_placeholder(placeholder), m_allowedChars(DEFAULT_ALLOWED_CHARS) {}

void TextInput::draw() {
    Frame::draw();

    auto sm = StyleManager::get();
    auto tm = TextureManager::get();
    auto rm = RenderManager::get();
    auto fontsize = tm->getFontBaseSize("font");
    auto textsize = rm->getTextSize(m_text, m_font, fontsize);
    auto cursorpos = rm->getTextSize(m_text.substr(0, m_cursorX), m_font, fontsize).x;
    auto mouse = GetMousePosition();
    auto color = COL_WHITE;

    if(m_text.empty()) color.a = 0x7F;

    auto textPos = Vec2f {m_border + 4.f, (m_bounds.height - fontsize) / 2};
    if(cursorpos > m_bounds.width - m_border * 2) {
        textPos.x -= cursorpos - m_bounds.width + m_border * 2.f + 8.f; 
    }

    auto bounds = getWorldBounds().to<Rectangle>();
    BeginScissorMode(bounds.x + m_border, bounds.y + m_border, bounds.width - m_border * 2.f, bounds.height - m_border * 2.f);
        rm->drawText(m_font, (m_text.empty() ? m_placeholder : m_text), textPos, color, fontsize);
        
        if(m_active) {
            rm->drawRect({textPos.x + cursorpos, textPos.y, 1.f, fontsize}, COL_WHITE);
        }
    EndScissorMode();
}

void TextInput::update() {
    auto mouse = GetMousePosition();
    auto sm = StyleManager::get();
    auto cursorhover = getWorldBounds().contains({mouse.x, mouse.y});
    m_color = sm->getValue<Col4u>(DEFAULT_UI_COLOR);

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        m_active = cursorhover;
    }

    if(cursorhover && !m_active) {
        m_color.brightness(-0.3f);
    }

    if(m_active) {
        m_color.brightness(-0.6f);
        auto isKeyHold = [](int key) -> bool { return IsKeyPressed(key) || IsKeyPressedRepeat(key); };
        char c = GetCharPressed();

        if(m_allowedChars.find(c) != std::string::npos && m_text.size() < m_maxChars) {
            m_text.insert(m_text.begin() + m_cursorX, c);
            m_cursorX++;
        }

        if(IsKeyDown(KEY_LEFT_CONTROL) && isKeyHold(KEY_V)) {
            auto text = std::string(GetClipboardText());
            m_text.append(text);
            m_cursorX += text.size();
        }

        if(IsKeyDown(KEY_LEFT_CONTROL) && isKeyHold(KEY_BACKSPACE)) {
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

        if(IsKeyPressed(KEY_HOME) || IsKeyPressed(KEY_UP)) {
            m_cursorX = 0;
        }

        if(IsKeyPressed(KEY_END) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_PAGE_DOWN)) {
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