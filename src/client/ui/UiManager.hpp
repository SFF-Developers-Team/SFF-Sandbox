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
    
public:
    UiManager();
    void drawMenuLogo();
};