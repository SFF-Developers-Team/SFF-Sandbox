#include <ui/nodes/Container.hpp>
#include <RenderManager.hpp>
#include <algorithm>
#include <raylib.h>
#include <rlgl.h>
#include <Logger.hpp>
#include <Game.hpp>
#include <raymath.h>

Container::Container() : Frame(), m_offset({0.f, 0.f}), m_frame(true) {}

void Container::alignItemsHorizontal(float padding) {
    if(!m_childs.empty()) {
        float width = padding * (m_childs.size() - 1);

        for(auto& child : m_childs) {
            width += child->getScaledWidth();
        }

        auto x = (getWidth() - width) / 2;

        for(auto& child : m_childs) {
            child->setPos({x, getHeight() / 2});
            x += child->getScaledWidth() + padding;
        }
    }
}

void Container::hugContent() {
    setSize({0.f, 0.f});

    for(auto& child : m_childs) {
        m_bounds.width = std::max(m_bounds.width, child->getX() + child->getWidth()); 
        m_bounds.height = std::max(m_bounds.height, child->getY() + child->getHeight()); 
    }
}

void Container::addChild(std::shared_ptr<Node> node) {
    node->m_parent = this;
    m_childs.push_back(node);
}

std::shared_ptr<Node> Container::getChild(std::string const& tag) {
    auto i = std::find_if(m_childs.begin(), m_childs.end(), [&tag](auto const& node) {
        return node->getTag() == tag;
    });

    return (i != m_childs.end() ? *i : nullptr);
}

bool Container::hasChild(std::string const& tag) {
    auto i = std::find_if(m_childs.begin(), m_childs.end(), [tag](auto& node) {
        return node->getTag() == tag;
    });

    return i != m_childs.end();
}

void Container::update() {
    Frame::update();

    for(auto& node : m_childs) {
        if(node->isEnabled()) {
            rlPushMatrix();
                rlTranslatef(node->getX() - m_offset.x, node->getY() - m_offset.y, 0.f);
                // rlRotatef(node->getRotation(), 0.f, 0.f, 1.f);
                // rlScalef((m_flipX ? -1.f : 1.f), (m_flipY ? -1.f : 1.f), 1.f);
                rlTranslatef(-(node->getAnchorX() * node->getScaledWidth()), -(node->getAnchorY() * node->getScaledHeight()), 0.f);
                rlScalef(node->getScaleX(), node->getScaleY(), 1.f);
                
                node->update();
            rlPopMatrix();
        }
    }
}

void Container::draw() {
    if(m_frame) {
        Frame::draw();
    }

    for(auto& node : m_childs) {
        if(node->isVisible()) {
            rlPushMatrix();
                rlTranslatef(node->getX() - m_offset.x, node->getY() - m_offset.y, 0.f);
                rlRotatef(node->getRotation(), 0.f, 0.f, 1.f);
                rlScalef((m_flipX ? -1.f : 1.f), (m_flipY ? -1.f : 1.f), 1.f);
                rlTranslatef(-(node->getAnchorX() * node->getScaledWidth()), -(node->getAnchorY() * node->getScaledHeight()), 0.f);
                rlScalef(node->getScaleX(), node->getScaleY(), 1.f);
                
                
                node->draw();
            rlPopMatrix();
        }
    }
}

void Container::sortChildsZ() {
    std::sort(m_childs.begin(), m_childs.end(), [](auto const& a, auto const& b) {
        return a->getZOrder() < b->getZOrder();
    });
}