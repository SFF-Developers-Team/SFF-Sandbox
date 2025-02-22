#pragma once
#include <ui/nodes/ScrollableContainer.hpp>

class ListContainer : public ScrollableContainer {
protected:
    float m_last;
    float m_padding;
    bool m_horizontal;
    bool m_autoResize;

public:
    ListContainer(bool horizontal, float padding = 2.5f);

    void addChild(std::shared_ptr<Node> node);
};