#pragma once
#include <ui/nodes/Frame.hpp>
#include <MiniFunction.hpp>

class DropDown : public Frame {
protected:
    std::vector<std::string> m_elements;
    MiniFunction<void(DropDown*, int)> m_callback;
    float m_maxHeight;
    bool m_opened;
    int m_selected;

public:
    DropDown(std::vector<std::string> const& elements, MiniFunction<void(DropDown*, int)> callback);

    void draw();
    void update();

    void setMaxHeight(float height);
    float getMaxHeight();
};