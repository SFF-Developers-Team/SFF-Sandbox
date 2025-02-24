#include <ui/nodes/Container.hpp>

class Layer : public Container {
private:
    using Container::setSize;
    using Container::setWidth;
    using Container::setHeight;
    using Container::setBorderWidth;

public:
    Layer();

    void draw();
    void update();
};