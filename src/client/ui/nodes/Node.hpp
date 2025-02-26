#pragma once
#include <Types.hpp>
#include <Utils.hpp>
#include <string>
#include <memory>
#include <vector>
#include <Debug.hpp>

enum NodeFlags : uint8_t {
    FLAG_GUI_SCALE = 1 << 0,
    FLAG_ALWAYS_CENTER = 1 << 1
};

class Container;

class Node {
    friend class Container;

private:
    Container* m_parent = nullptr;
    int m_zOrder;

protected:
    std::string m_tag;
    Rectf m_bounds;
    Vec2f m_anchor;
    Vec2f m_scale;
    Col4u m_color;
    uint8_t m_flags;
    float m_rotation;
    bool m_visible;
    bool m_enabled;
    bool m_flipX;
    bool m_flipY;

public:
    Node();

    virtual void update() {}
    virtual void draw();

    void setTag(std::string const& tag) { m_tag = tag; }
    std::string const& getTag() { return m_tag; }

    Vec2f getPos() { return Vec2f {m_bounds.x, m_bounds.y}; }
    float getX() { return m_bounds.x; }
    float getY() { return m_bounds.y; }

    void setPos(Vec2f pos) { m_bounds.x = pos.x; m_bounds.y = pos.y; }
    void setX(float x) { m_bounds.x = x; }
    void setY(float y) { m_bounds.y = y; }

    Vec2f getSize() { return Vec2f {m_bounds.width, m_bounds.height}; }
    float getWidth() { return m_bounds.width; }
    float getHeight() { return m_bounds.height; }

    Vec2f getScaledSize() { return Vec2f {m_bounds.width * m_scale.x, m_bounds.height * m_scale.y}; }
    float getScaledWidth() { return m_bounds.width * m_scale.x; }
    float getScaledHeight() { return m_bounds.height * m_scale.y; }

    void setSize(Vec2f size) { m_bounds.width = size.x; m_bounds.height = size.y; }
    void setWidth(float w) { m_bounds.width = w; }
    void setHeight(float h) { m_bounds.height = h; }

    Vec2f getAnchor() { return m_anchor; }
    float getAnchorX() { return m_anchor.x; }
    float getAnchorY() { return m_anchor.y; }

    void setAnchor(Vec2f anchor) { m_anchor = anchor; }
    void setAnchorX(float x) { m_anchor.x = x; }
    void setAnchorY(float y) { m_anchor.y = y; }

    void setZOrder(int z);
    int getZOrder() { return m_zOrder; }

    Col4u getColor() { return m_color; }
    void setColor(Col4u color) { m_color = color; }

    void setVisible(bool flag) { m_visible = flag; }
    void setEnabled(bool flag) { m_enabled = flag; }
    bool isVisible() { return m_visible; }
    bool isEnabled() { return m_enabled; }

    Container* getParent() { return m_parent; }
    Rectf getWorldBounds();
    Vec2f getLocalMousePosition();

    void setScale(float scale) { m_scale = {scale, scale}; }
    Vec2f getScale() { return m_scale; }

    void setScaleX(float x) { m_scale.x = x; }
    void setScaleY(float y) { m_scale.y = y; }
    float getScaleX() { return m_scale.x; }
    float getScaleY() { return m_scale.y; }

    float getGlobalScaleX();
    float getGlobalScaleY();

    void setFlags(uint8_t flags) { m_flags |= flags; }
    void clearFlags(uint8_t flags) { m_flags &= ~flags; }
    bool getFlag(uint8_t flag) { return m_flags & flag; }
    NodeFlags getFlags(uint8_t flag) { return static_cast<NodeFlags>(m_flags); }

    bool isMouseHover();

    void setFlipX(bool flag) { m_flipX = flag; }
    void setFlipY(bool flag) { m_flipY = flag; }
    float getFlipX() { return m_flipX; }
    float getFlipY() { return m_flipY; }

    float getRotation() { return m_rotation; }
    void setRotation(float degrees) { m_rotation = degrees; }
    
    float getLeftX() { return m_bounds.x - m_bounds.width + m_bounds.width * m_scale.x * m_anchor.x; } 
    float getRightX() { return m_bounds.x + m_bounds.width - m_bounds.width * m_scale.x * m_anchor.x; }
    float getTopY() { return m_bounds.y - m_bounds.height + m_bounds.height * m_scale.y * m_anchor.y; }
    float getBottomY() { return m_bounds.y + m_bounds.height - m_bounds.height * m_scale.y * m_anchor.y; } 
};