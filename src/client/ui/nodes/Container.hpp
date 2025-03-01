#pragma once
#include <ui/nodes/Frame.hpp>

class Container : public Frame {
protected:
    std::vector<std::shared_ptr<Node>> m_childs;
    Vec2f m_offset;
    bool m_frame;

public:
    Container();

    virtual void update();
    virtual void draw();

    void alignItemsHorizontal(float padding);

    void addChild(std::shared_ptr<Node> node);
    bool hasChild(size_t index) { return index < m_childs.size(); }
    bool hasChild(std::string const& tag);
    std::shared_ptr<Node> getChild(size_t index) { return m_childs[index]; }
    std::shared_ptr<Node> getChild(std::string const& tag);
    std::vector<std::shared_ptr<Node>> const& getChildren() { return m_childs; }

    void sortChildsZ();
    void hugContent();

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

    bool isFrameVisible() { return m_frame; }
    void setFrameVisible(bool frame) { m_frame = frame; }

    std::shared_ptr<Node> operator[](size_t index) { return getChild(index); }
    std::shared_ptr<Node> operator[](std::string const& tag) { return getChild(tag); }
};