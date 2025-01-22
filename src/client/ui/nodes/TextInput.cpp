#include <ui/nodes/TextInput.hpp>
#include <StyleManager.hpp>
#include <raylib.h>

TextInput::TextInput() : Node() {
    m_allowedChars = " !@#$%^&*()\\/:;\"'№?abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
}

void TextInput::draw() {
    auto sm = StyleManager::get();
    auto fontSize = sm->getValue<float>(TEXT_SIZE);
    auto borderw = sm->getValue<float>(BORDER_WIDTH);
    auto width = MeasureText(m_text.c_str(), fontSize);
    auto mouse = GetMousePosition();
    auto bounds = getBoundsAnchor();
    auto cursorPos = MeasureText(m_text.substr(0, m_cursorX).c_str(), fontSize);

    auto first = FIRST_COLOR_NORMAL;
    auto second = SECOND_COLOR_NORMAL;

    if(bounds.contains({mouse.x, mouse.y}) && IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        first = FIRST_COLOR_FOCUS;
        second = SECOND_COLOR_FOCUS;
    }

    if(m_active) {
        first = FIRST_COLOR_PRESS;
        second = SECOND_COLOR_PRESS;
    }

    auto bnd = bounds.to<Rectangle>();
    auto textPos = Vec2f {bnd.x + borderw + 4, bnd.y + bnd.height / 2 - fontSize / 2};
    auto textCol = sm->getValue<Col4u>(TEXT_COLOR_NORMAL).to<Color>();
    
    DrawRectangleRec(bnd, sm->getValue<Col4u>(first).to<Color>());
    DrawRectangleLinesEx(bnd, borderw, sm->getValue<Col4u>(second).to<Color>());
    DrawText(m_text.c_str(), textPos.x, textPos.y, fontSize, textCol);
    
    if(m_active) {
        DrawText("|", textPos.x + cursorPos, textPos.y, fontSize, textCol);
    }
}

void TextInput::update() {
    auto mouse = GetMousePosition();
    auto bounds = getBoundsAnchor();

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        m_active = bounds.contains({mouse.x, mouse.y});
    }

    if(m_active) {
        char c = GetCharPressed();

        if(m_allowedChars.find(c) != std::string::npos && m_text.size() < m_maxChars) {
            m_text.append(1, c);
            m_cursorX++;
        }

        if(IsKeyPressed(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
            m_text.append(GetClipboardText());
        }

        if(IsKeyPressed(KEY_BACKSPACE) && m_cursorX > 0) {
            m_text.erase(m_cursorX - 1, 1);
            m_cursorX--;
        }

        if(IsKeyPressed(KEY_DELETE) && m_cursorX < m_text.size()) {
            m_text.erase(m_cursorX, 1);
        }

        if(IsKeyPressed(KEY_HOME) || IsKeyPressed(KEY_UP)) {
            m_cursorX = 0;
        }

        if(IsKeyPressed(KEY_END) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_PAGE_DOWN)) {
            m_cursorX = m_text.size();
        }

        if(IsKeyPressed(KEY_LEFT) && m_cursorX > 0) {
            m_cursorX--;
        }

        if(IsKeyPressed(KEY_RIGHT) && m_cursorX < m_text.size()) {
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