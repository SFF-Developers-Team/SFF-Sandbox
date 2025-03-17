#include <ui/basic/Frame.hpp>
#include <MiniFunction.hpp>
#include <vector>
#include <algorithm>

class Block;
class LocalPlayer;

class Hotbar : public Frame {
private:
    using Frame::setSize;
    using Frame::setWidth;
    using Frame::setHeight;

protected:
    std::shared_ptr<LocalPlayer> m_player;
    MiniFunction<void()> m_invCallback;

public:
    Hotbar(std::shared_ptr<LocalPlayer> player, MiniFunction<void()> const& inventoryCallback);

    void draw();
    void update();
};