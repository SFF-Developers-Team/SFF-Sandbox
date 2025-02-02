#include <ui/nodes/Container.hpp>
#include <rlgl.h>

void Container::alignItemsHorizontal(float padding) {
    float width = 0.f;

    for(auto& child : m_childs) {
        width += (child->getWidth() * child->getAnchorX()) + padding;
    }

    auto x = (getWidth() - width) / 2;

    for(auto& child : m_childs) {
        child->setPos({x, (getHeight() - child->getHeight()) / 2});
        x += child->getWidth() + padding * child->getAnchorX();
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

void Container::update() {
    for(auto& node : m_childs) {
        if(node->isEnabled()) {
            node->update();
        }
    }
}

void Container::draw() {
    Frame::draw();

    for(auto& node : m_childs) {
        if(node->isVisible()) {
            rlPushMatrix();
                rlTranslatef(node->getX(), node->getY(), 0.f);
                rlTranslatef(-(node->getAnchorX() * node->getWidth()), -(node->getAnchorY() * node->getHeight()), 0.f);
                
                node->draw();
            rlPopMatrix();
        }
    }
}

std::vector<std::shared_ptr<Node>> const& Container::getChildren() {
    return m_childs;
}