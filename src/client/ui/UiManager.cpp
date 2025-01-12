#include <TextureManager.hpp>
#include <RenderManager.hpp>
#include <ui/UiManager.hpp>
#include <raygui.h>
#include <Logger.hpp>
#include <string>
#include <cmath>

UiManager::UiManager() {}

void UiManager::drawText(std::string const& str, Vec2f pos, float fontSize, bool centered, Col4u color) {
    auto width = MeasureText(str.c_str(), fontSize);
    DrawText(str.c_str(), pos.x - width * (centered ? 0.5f : 0.f), pos.y, fontSize, color.to<Color>());
}

void UiManager::drawMenuLogo() {
    auto rm = RenderManager::get();
    auto tm = TextureManager::get();

    auto sw = static_cast<float>(GetScreenWidth());

    auto sff = tm->getTexture("sff.png");
    auto sffY = 100.f + ((float)sin(GetTime()) * 30);
    rm->drawTexture("sff.png", {sw / 2 - sff.width / 2, sffY, static_cast<float>(sff.width), static_cast<float>(sff.height)});

    auto text = "Sandbox";
    auto textH = 35;
    auto textW = MeasureText(text, textH);
    drawText(text, {sw / 2 - textW / 2, sffY + sff.width * 0.7f}, 35);
}

void UiManager::drawButton(std::string const& label, Rectf dest, MiniFunction<void()> const& callback) {
    if(GuiButton(dest.to<Rectangle>(), label.c_str())) {
        callback();
    }
}

void UiManager::drawButtonsV(Vec2f start, Vec2f btnSize, float padding, std::initializer_list<BtnPair> const& btns) {
    for(auto const& btn : btns) {
        drawButton(btn.label, {start.x, start.y, btnSize.x, btnSize.y}, btn.callback);
        start.y += padding;
    }
}

void UiManager::drawInput(std::string& out, Rectf dest, std::string const& caption, float fontSize) {
    Vec2f mouse = {
        static_cast<float>(GetMouseX()), 
        static_cast<float>(GetMouseY())
    };

    if(dest.contains(mouse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        m_selected = m_nodeCount;
    }

    if(!caption.empty()) {
        drawText(caption, {dest.x, dest.y - fontSize - 2.f}, fontSize);
    }

    if(m_selected != m_nodeCount) {
        DrawRectangleRec(dest.to<Rectangle>(), GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_NORMAL)));
    }

    GuiTextBox(dest.to<Rectangle>(), out.data(), out.size(), m_selected == m_nodeCount);

    m_nodeCount++;
}

void UiManager::drawSliderBar(Rectf dest, std::string const& strLeft, std::string const& strRight, float* value, float minValue, float maxValue) {
    GuiSlider(dest.to<Rectangle>(), strLeft.c_str(), strRight.c_str(), value, minValue, maxValue);
}

void UiManager::drawCheckBox(Rectf dest, std::string const& str, bool* check) {
    GuiCheckBox(dest.to<Rectangle>(), str.c_str(), check);
}

void UiManager::drawDropDownBox(std::string& str, Rectf dest, int active, bool edit) {
    GuiDropdownBox(dest.to<Rectangle>(), str.c_str(), &active, edit);
}

void UiManager::drawGuiListView(const std::string& str, Rectf dest, int scrollIndex, int active) {
    GuiListView(dest.to<Rectangle>(), str.c_str(), &scrollIndex, &active);
}