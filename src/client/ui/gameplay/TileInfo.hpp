#include <ui/basic/Frame.hpp>

class Tile;

class TileInfo : public Frame {
private:
    using Frame::setSize;
    using Frame::setWidth;
    using Frame::setHeight;

public:
    TileInfo() { setSize({140.f, 40.f}); }
    
    void draw();
};