#include <ui/nodes/ListContainer.hpp>
#include <StyleManager.hpp>

ListContainer::ListContainer(bool horizontal, float padding) : ScrollableContainer(), m_horizontal(horizontal), m_last(m_border), m_padding(padding) {
    auto size = StyleManager::get()->getValue<Vec2f>(DEFAULT_ELEMENT_SIZE);
    setSize({size.x + m_border * 4, size.y + m_border * 4 * (!m_horizontal ? 10.f : 1.f)});
}

void ListContainer::addChild(std::shared_ptr<Node> node) {
    auto x = (m_horizontal ? m_last : (m_border * 2)) + node->getScaledWidth() * node->getAnchorX();
    auto y = (!m_horizontal ? m_last : (m_border * 2)) + node->getScaledHeight() * node->getAnchorY();

    node->setPos({x, y});
    m_last += (m_horizontal ? node->getScaledWidth() : node->getScaledHeight()) + m_padding;

    Container::addChild(node);
}