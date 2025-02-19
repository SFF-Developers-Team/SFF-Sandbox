#pragma once
#include <ui/nodes/Frame.hpp>
#include <MiniFunction.hpp>
#include <vector>

class List : public Frame {
protected:
    std::vector<std::string> m_elements;
    MiniFunction<void(List*, int)> m_callback;
    float m_scrollOffset;

public:
    List(std::vector<std::string> const& elements, MiniFunction<void(List*, int)> callback);

    void update();
    void draw();
};