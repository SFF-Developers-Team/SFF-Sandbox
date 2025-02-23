#pragma once
#include <ui/nodes/ScrollableContainer.hpp>

class ListContainer : public ScrollableContainer {
protected:
    float m_last;
    float m_padding;
    bool m_horizontal;
    bool m_autoResize;
    bool m_resizeElements;

public:
    ListContainer(bool horizontal, bool resizeElements = true, float padding = 2.5f);

    void addChild(std::shared_ptr<Node> node);
};