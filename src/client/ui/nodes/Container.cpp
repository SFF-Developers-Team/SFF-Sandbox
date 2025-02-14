#include <ui/nodes/Container.hpp>
#include <RenderManager.hpp>
#include <algorithm>
#include <raylib.h>
#include <rlgl.h>
#include <Logger.hpp>
#include <Game.hpp>

Container::Container() : Frame(), m_scrollOffset(0.f), m_scrollable(false) {}

void Container::alignItemsHorizontal(float padding) {
    if(m_childs.empty()) {
        return;
    }
    
    float width = 0.f;

    for(auto& child : m_childs) {
        width += child->getWidth();
    }

    auto x = (getWidth() - width) / 2 + m_childs[0]->getWidth() * m_childs[0]->getAnchorX();

    for(auto& child : m_childs) {
        child->setPos({x, getHeight() / 2});
        x += child->getWidth();
    }
}

void Container::addChild(std::shared_ptr<Node> node) {
    node->m_parent = this;
    m_childs.push_back(node);
}

std::shared_ptr<Node> Container::getChild(size_t index) {
    return m_childs[index];
}

std::shared_ptr<Node> Container::getChild(std::string const& tag) {
    auto i = std::find_if(m_childs.begin(), m_childs.end(), [&tag](auto const& node) {
        return node->getTag() == tag;
    });

    return (i != m_childs.end() ? *i : nullptr);
}

bool Container::hasChild(size_t index) {
    return index < m_childs.size();
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
            node->update();
        }
    }

    if(m_scrollable) {
        auto bounds = getWorldBounds();
        auto mouse = GetMousePosition();
        auto totalHeight = calculateTotalHeight();

        if(totalHeight > m_bounds.height) {
            if(bounds.contains({mouse.x, mouse.y}) && GetMouseWheelMove() != 0.f) {
                m_scrollOffset -= GetMouseWheelMove() * 10.f;
            }

            m_scrollOffset = std::clamp(m_scrollOffset, 0.f, totalHeight - bounds.height + m_border);
        }
    }
}

void Container::draw() {
    Frame::draw();
    auto rm = RenderManager::get();

    auto const bounds = getWorldBounds();
    auto const totalHeight = calculateTotalHeight();
    auto const contentHeight = bounds.height - m_border * 2;
    auto const borderColor = m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F};
    auto const scrollBar = totalHeight > bounds.height && m_scrollable;

    BeginScissorMode(bounds.x + m_border, bounds.y + m_border, bounds.width - m_border * 2, contentHeight);
        for(auto& node : m_childs) {
            if(node->isVisible()) {
                rlPushMatrix();
                    rlTranslatef(node->getX(), node->getY() - m_scrollOffset, 0.f);
                    rlTranslatef(-(node->getAnchorX() * node->getWidth()), -(node->getAnchorY() * node->getHeight()), 0.f);
                    rlTranslatef(node->getScaleX(), node->getScaleY(), 0.f);
                    
                    node->draw();
                rlPopMatrix();
                
                auto nodeb = node->getWorldBounds();
                auto myb = getWorldBounds();
                nodeb.x -= myb.x;
                nodeb.y -= myb.y; 
                DrawRectangleLinesEx(nodeb.to<Rectangle>(), 1.f, GREEN);
            }
        }

        if(scrollBar) {
            rm->drawRect({bounds.width - m_border * 2, m_border + (m_scrollOffset / totalHeight) * contentHeight, m_border, (contentHeight / totalHeight) * contentHeight}, borderColor);
        }
    EndScissorMode();
}

bool Container::isScrollable() {
    return m_scrollable;
}

void Container::setScrollable(bool flag) {
    m_scrollable = flag;

    if(!m_scrollable) {
        m_scrollOffset = false;
    }
}

void Container::resetScroll() {
    m_scrollOffset = 0.f;
}

std::vector<std::shared_ptr<Node>> const& Container::getChildren() {
    return m_childs;
}

void Container::sortChildsZ() {
    std::sort(m_childs.begin(), m_childs.end(), [](auto const& a, auto const& b) {
        return a->getZOrder() < b->getZOrder();
    });
}