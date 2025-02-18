#pragma once
#include <ui/nodes/Frame.hpp>

class Container : public Frame {
protected:
    std::vector<std::shared_ptr<Node>> m_childs;
    float m_scrollOffset;
    bool m_scrollable;

    float calculateTotalHeight();
public:
    Container();

    void update();
    void draw();

    void alignItemsHorizontal(float padding);
    bool isScrollable() { return m_scrollable; }
    void setScrollable(bool flag);

    void resetScroll() { m_scrollOffset = 0.f; }

    void addChild(std::shared_ptr<Node> node);
    bool hasChild(size_t index) { return index < m_childs.size(); }
    bool hasChild(std::string const& tag);
    std::shared_ptr<Node> getChild(size_t index) { return m_childs[index]; }
    std::shared_ptr<Node> getChild(std::string const& tag);
    std::vector<std::shared_ptr<Node>> const& getChildren() { return m_childs; }

    void sortChildsZ();

    template<typename T>
    std::shared_ptr<T> getChild(size_t index) {
        auto ptr = std::static_pointer_cast<T>(getChild(index));

        if(!ptr) {
            static_assert("Incompatible Node type!");
        }

        return ptr;
    }

    template<typename T>
    std::shared_ptr<T> getChild(std::string const& tag) {
        auto ptr = std::static_pointer_cast<T>(getChild(tag));

        if(!ptr) {
            static_assert("Incompatible Node type!");
        }

        return ptr;
    }
};