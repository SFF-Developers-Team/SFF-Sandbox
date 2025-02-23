#include <ui/nodes/Frame.hpp>

class Block;

class BlockInfo : public Frame {
private:
    using Frame::setSize;

public:
    BlockInfo() {};
    
    void draw();
    void update();
};