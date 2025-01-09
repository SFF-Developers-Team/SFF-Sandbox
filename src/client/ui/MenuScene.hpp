#pragma once
#include <ui/Scene.hpp>
#include <raylib.h>

class MenuScene : public Scene {
private:
    RenderTexture2D m_bgRender;

public:
    MenuScene();
    
    void draw() override;
    void update() override {}
};