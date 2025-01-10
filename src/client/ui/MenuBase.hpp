#pragma once
#include <ui/Scene.hpp>
#include <raylib.h>

class MenuBase : public Scene {
private:
    RenderTexture2D m_bgRender;

public:
    MenuBase();
    
    void draw() override;
};