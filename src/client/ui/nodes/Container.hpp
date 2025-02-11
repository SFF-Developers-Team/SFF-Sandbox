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
    bool isScrollable();
    void setScrollable(bool flag);

    void resetScroll();

    void addChild(std::shared_ptr<Node> node);
    bool hasChild(size_t index);
    bool hasChild(std::string const& tag);
    std::shared_ptr<Node> getChild(size_t index);
    std::shared_ptr<Node> getChild(std::string const& tag);
    std::vector<std::shared_ptr<Node>> const& getChildren();

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