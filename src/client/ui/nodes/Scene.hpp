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

    using Container::setPos;
    using Container::setX;
    using Container::setY;
    using Container::setScale;
    using Container::setScaleX;
    using Container::setScaleY;
    using Container::setAnchor;
    using Container::setAnchorX;
    using Container::setAnchorY;
    using Container::setBorderWidth;

public:
    Scene();
    
    virtual void update();

    virtual void onPush() {}
    virtual void onPop() {}

    virtual void onShow();
    virtual void onHide() {}
    
    void destroy() { m_destroy = true; }
    bool shouldDestroy() { return m_destroy; }

    void keyBackClicked();
};