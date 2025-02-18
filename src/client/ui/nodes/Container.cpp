#include <ui/nodes/Container.hpp>
#include <RenderManager.hpp>
#include <algorithm>
#include <raylib.h>
#include <rlgl.h>
#include <Logger.hpp>
#include <Game.hpp>
#include <raymath.h>

Container::Container() : Frame(), m_scrollOffset(0.f), m_scrollable(false) {}

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

float Container::calculateTotalHeight() {
    float ret = 0.f;
    
    for(auto& node : m_childs) {
        ret = std::max(ret, node->getY() + node->getHeight());
    }

    return ret;
}

void Container::update() {
    for(auto& node : m_childs) {
        if(node->isEnabled()) {
            rlPushMatrix();
                rlTranslatef(node->getX(), node->getY() - m_scrollOffset, 0.f);
                rlTranslatef(-(node->getAnchorX() * node->getScaledWidth()), -(node->getAnchorY() * node->getScaledHeight()), 0.f);
                rlScalef(node->getScaleX(), node->getScaleY(), 0.f);
                
                node->update();
            rlPopMatrix();
        }
    }

    if(m_scrollable) {
        auto const totalHeight = calculateTotalHeight();
        auto const contentHeight = (m_bounds.height - m_border * 2);

        if(totalHeight > m_bounds.height) {
            if(isMouseHover() && GetMouseWheelMove() != 0.f) {
                m_scrollOffset -= GetMouseWheelMove() * 10.f;
            }

            m_scrollOffset = std::clamp(m_scrollOffset, 0.f, totalHeight - contentHeight);
        }
    }
}

void Container::draw() {
    Frame::draw();
    auto const bounds = getWorldBounds();
    auto const totalHeight = calculateTotalHeight();
    auto const contentHeight = m_bounds.height - m_border * 2;
    auto const borderColor = m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F};
    auto const scrollBar = totalHeight > bounds.height && m_scrollable;

    Rectf const cutRect = {
        bounds.x + m_border * getGlobalScaleX(),
        bounds.y + m_border * getGlobalScaleY(),
        bounds.width - (m_border * 2) * getGlobalScaleX(),
        contentHeight * getGlobalScaleY()
    };

    BeginScissorMode(cutRect.x, cutRect.y, cutRect.width, cutRect.height);
        for(auto& node : m_childs) {
            if(node->isVisible()) {
                rlPushMatrix();
                    rlTranslatef(node->getX(), node->getY() - m_scrollOffset, 0.f);
                    rlTranslatef(-(node->getAnchorX() * node->getWidth()), -(node->getAnchorY() * node->getHeight()), 0.f);
                    rlScalef(node->getScaleX(), node->getScaleY(), 0.f);
                    
                    node->draw();
                rlPopMatrix();
            }
        }

        if(scrollBar) {
            RenderManager::drawRect({m_bounds.width - m_border * 2, m_border + (m_scrollOffset / totalHeight) * contentHeight, m_border, (contentHeight / totalHeight) * contentHeight}, borderColor);
        }
    EndScissorMode();
}

void Container::setScrollable(bool flag) {
    m_scrollable = flag;

    if(!m_scrollable) {
        m_scrollOffset = 0.f;
    }
}

void Container::sortChildsZ() {
    std::sort(m_childs.begin(), m_childs.end(), [](auto const& a, auto const& b) {
        return a->getZOrder() < b->getZOrder();
    });
}