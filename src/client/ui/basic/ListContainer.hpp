#pragma once
#include <ui/basic/ScrollableContainer.hpp>

class ListContainer : public ScrollableContainer {
protected:
    float m_last;
    float m_padding;
    bool m_horizontal;
    bool m_autoResize;
    bool m_resizeElements;
    bool m_selfAutoResize;

public:
    ListContainer(bool horizontal, bool resizeElements = true, bool selfAutoResize = false, float padding = 2.5f);

    void addChild(std::shared_ptr<Node> node);
};