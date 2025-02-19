#include <ui/nodes/Frame.hpp>
#include <vector>
#include <algorithm>

class Block;
class Player;

class Hotbar : public Frame {
private:
    using Frame::setSize;

protected:
    std::shared_ptr<Player> m_player;

public:
    Hotbar(std::shared_ptr<Player> player);

    void draw();
};