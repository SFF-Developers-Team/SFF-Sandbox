#pragma once
#include <ui/UI.hpp>
#include <raylib.h>

class MenuBase : public Scene {
public:
    MenuBase();
    
    virtual void draw() override;
};