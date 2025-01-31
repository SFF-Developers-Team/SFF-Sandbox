#pragma once
#include <ui/nodes/Container.hpp>
#include <Types.hpp>
#include <Game.hpp>
#include <raylib.h>

class Scene : public Container {
private:
    bool m_destroy = false;

public:
    Scene();
    
    virtual void update();
    virtual void draw();

    virtual void onPush() {}
    virtual void onPop() {}
    
    void destroy();
    bool shouldDestroy();
};