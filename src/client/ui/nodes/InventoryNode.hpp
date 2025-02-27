#include <ui/nodes/Frame.hpp>
#include <vector>
#include <memory>
#include <entity/Player.hpp>

class InventoryNode : public Frame {
private:
    std::shared_ptr<Player> m_player;
    std::shared_ptr<InventoryItem> m_selected;

    using Frame::setSize;

    Rectf getCellPosition(int index);

public:
    InventoryNode(std::shared_ptr<Player> player);

    void draw();
    void update();

    void setEnabled(bool);
};