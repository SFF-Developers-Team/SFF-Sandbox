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
public:
    UiManager();

    void drawText(std::string const& str, Vec2f pos, float size = 11.f, Col4u color = COL_WHITE);
    void drawButton(std::string const& label, Rectf dest, MiniFunction<void()> const& callback);
    void drawButtonsV(Vec2f start, Vec2f btnSize, float padding, std::initializer_list<BtnPair> const& btns);
    void drawMenuLogo();
};