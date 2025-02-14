#pragma once
#include <Types.hpp>
#include <Utils.hpp>
#include <string>
#include <memory>
#include <vector>

class Container;

class Node {
    friend class Container;

private:
    Container* m_parent = nullptr;
    Rectf m_bounds;
    int m_zOrder;

protected:
    std::string m_tag;
    Vec2f m_anchor;
    Vec2f m_scale;
    Col4u m_color;
    bool m_visible;
    bool m_enabled;

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

    void setZOrder(int z);
    int getZOrder();

    Col4u getColor();
    void setColor(Col4u color);

    void setVisible(bool flag);
    bool isVisible();

    void setEnabled(bool flag);
    bool isEnabled();

    Container* getParent();
    Rectf getWorldBounds();

    void setScale(Vec2f scale);
    Vec2f getScale();

    void setScaleX(float x);
    float getScaleX();

    void setScaleY(float y);
    float getScaleY();
};