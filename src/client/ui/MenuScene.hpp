#pragma once
#include <ui/Scene.hpp>

class MenuScene : public Scene {
public:
    MenuScene();
    
    void draw() override;
    void update() override {}
};