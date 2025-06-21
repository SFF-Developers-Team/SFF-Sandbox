#pragma once
#include <ui/basic/List.hpp>
#include <MiniFunction.hpp>

class DropDown : public List {
protected:
    std::string m_mask;
    float m_maxHeight;
    bool m_opened;

public:
    DropDown(std::vector<std::string> const& elements, MiniFunction<void(List*, int)> callback);

    void draw();
    void update();

    void setMaxHeight(float height) { m_maxHeight = height; }
    float getMaxHeight() { return m_maxHeight; }

    float getContentHeight() override;

    void setMask(std::string const& mask) { m_mask = mask; }
    std::string const& getMask() { return m_mask; }
};