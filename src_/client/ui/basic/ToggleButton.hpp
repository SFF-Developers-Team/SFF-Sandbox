#include <ui/basic/Frame.hpp>
#include <MiniFunction.hpp>

class ToggleButton : public Frame {
protected:
    MiniFunction<void(ToggleButton*, bool)> m_callback;
    std::string m_text;
    float m_fontSize;
    bool m_flag;

    virtual void callback();
    
public:
    ToggleButton(std::string const& text, MiniFunction<void(ToggleButton*, bool)> const& callback);

    void draw();
    void update();

    bool getValue() { return m_flag; }
    void setValue(bool value) { m_flag = value; }

    void setText(std::string const& text) { m_text = text; }
};