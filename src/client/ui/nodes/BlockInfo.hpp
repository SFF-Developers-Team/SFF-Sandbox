#include <ui/nodes/Frame.hpp>

class Block;

class BlockInfo : public Frame {
private:
    using Frame::setSize;
    using Frame::setWidth;
    using Frame::setHeight;

public:
    BlockInfo() { setSize({140.f, 40.f}); }
    
    void draw();
};