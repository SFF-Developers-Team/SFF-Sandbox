#pragma once
#include <ui/nodes/Frame.hpp>
#include <MiniFunction.hpp>

class DropDown : public Frame {
protected:
    MiniFunction<void(DropDown*, int)> m_callback;
    std::vector<std::string> m_elements;
    std::string m_mask;
    float m_maxHeight;
    float m_scrollOffset;
    bool m_opened;
    int m_selected;

public:
    DropDown(std::vector<std::string> const& elements, MiniFunction<void(DropDown*, int)> callback);

    void draw();
    void update();

    void setMaxHeight(float height);
    float getMaxHeight();

    void setMask(std::string const& mask);
    std::string const& getMask();
};