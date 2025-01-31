#pragma once
#include <ui/UI.hpp>
#include <raylib.h>

class MenuBase : public Scene {
private:
    RenderTexture2D m_bgRender;

public:
    MenuBase();
    ~MenuBase();
    
    void draw() override;
};