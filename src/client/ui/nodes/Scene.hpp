#pragma once
#include <ui/nodes/Container.hpp>
#include <Types.hpp>
#include <Game.hpp>
#include <raylib.h>

class Scene : public Container {
protected:
    bool m_keyBack = true;

private:
    bool m_destroy = false;

public:
    Scene();
    
    virtual void update();
    virtual void draw();

    virtual void onPush() {}
    virtual void onPop() {}

    virtual void onShow();
    virtual void onHide() {}
    
    void destroy();
    bool shouldDestroy();
};