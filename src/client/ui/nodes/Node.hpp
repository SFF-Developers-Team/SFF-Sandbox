#pragma once
#include <Types.hpp>
#include <Utils.hpp>
#include <string>

class Node {
protected:
    Rectf m_bounds;
    Vec2f m_anchor;
    std::string m_tag;

    Rectf getBoundsAnchor();

public:
    Node();

    virtual void update() = 0;
    virtual void draw() = 0;

    void setTag(std::string const& tag);
    std::string const& getTag();

    Vec2f getPos();
    float getX();
    float getY();

    void setPos(Vec2f pos);
    void setX(float x);
    void setY(float y);

    Vec2f getSize();
    float getWidth();
    float getHeight();

    void setSize(Vec2f size);
    void setWidth(float w);
    void setHeight(float h);

    Vec2f getAnchor();
    float getAnchorX();
    float getAnchorY();

    void setAnchor(Vec2f anchor);
    void setAnchorX(float x);
    void setAnchorY(float y);
};