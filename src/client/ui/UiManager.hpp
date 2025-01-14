#pragma once
#include <memory>
#include <string>
#include <functional>
#include <Types.hpp>
#include <MiniFunction.hpp>

struct BtnPair {
    std::string const& label;
    MiniFunction<void()> const& callback;
};

class UiManager {
private:
    int m_nodeCount = 0;
    int m_selected = -1;

    friend class Scene;

public:
    UiManager();

    void drawText(std::string const& str, Vec2f pos, float size = 11.f, bool centered = false, Col4u color = COL_WHITE);
    void drawButton(std::string const& label, Rectf dest, MiniFunction<void()> const& callback);
    void drawButtonsV(Vec2f start, Vec2f btnSize, float padding, std::initializer_list<BtnPair> const& btns);
    void drawSliderBar(Rectf dest, std::string const& strLeft, std::string const& strRight, float* value, float minValue, float maxValue);
    void drawCheckBox(Rectf dest, std::string const& str, bool* check);
    void drawInput(std::string& out, Rectf dest, std::string const& caption, float fontSize = 11.f);
    void drawDropDownBox(std::string& str, Rectf dest, int active, bool edit);
    void drawGuiListView(const std::string& str, Rectf dest, int scrollIndex, int active);
    void drawMenuLogo();
};