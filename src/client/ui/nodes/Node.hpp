#pragma once
#include <Types.hpp>
#include <Utils.hpp>
#include <string>
#include <memory>
#include <vector>

class Container;

class Node {
    friend class Container;

protected:
    Container* m_parent = nullptr;
    std::string m_tag;
    Rectf m_bounds;
    Vec2f m_anchor;
    Col4u m_color;

    Rectf getRealBounds();

public:
    Node();

    virtual void update();
    virtual void draw();

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

    Col4u getColor();
    void setColor(Col4u color);

    Container* getParent();
};