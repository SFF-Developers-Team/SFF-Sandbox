#include <ui/basic/ListContainer.hpp>
#include <StyleManager.hpp>

ListContainer::ListContainer(bool horizontal, bool resizeElements, bool selfAutoResize, float padding) 
    : ScrollableContainer(), m_horizontal(horizontal), m_last(m_border * 2), m_resizeElements(resizeElements), m_padding(padding), m_selfAutoResize(selfAutoResize) {
    auto size = StyleManager::get()->getValue<Vec2f>(DEFAULT_ELEMENT_SIZE);
    setSize({size.x + m_border * 4, size.y + m_border * 4 * (!m_horizontal ? 10.f : 1.f)});
}

void ListContainer::addChild(std::shared_ptr<Node> node) {
    if(m_resizeElements) {
        (m_horizontal) ? node->setHeight(getHeight() - m_border * 4) : node->setWidth(getWidth() - m_border * 4);
    }

    auto x = (m_horizontal ? m_last : (m_border * 2)) + node->getScaledWidth() * node->getAnchorX();
    auto y = (!m_horizontal ? m_last : (m_border * 2)) + node->getScaledHeight() * node->getAnchorY();

    node->setPos({x, y});

    m_last += (m_horizontal ? node->getScaledWidth() : node->getScaledHeight()) + m_padding;

    if(m_selfAutoResize) {
        (m_horizontal ? setWidth(m_last - m_padding) : setHeight(m_last - m_padding));
    }

    Container::addChild(node);
}