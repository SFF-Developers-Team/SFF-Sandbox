#include <ui/nodes/Node.hpp>

class Text : public Node {
protected:
    std::string m_text;
    float m_size;

public:
    Text(std::string const& text, float size = 11.f);

    void draw();
    void update() {}

    void setText(std::string const& text);
};