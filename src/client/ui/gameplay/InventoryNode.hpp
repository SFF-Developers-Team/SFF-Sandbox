#include <ui/basic/Frame.hpp>
#include <vector>
#include <memory>
#include <entity/LocalPlayer.hpp>

class InventoryNode : public Frame {
private:
    std::shared_ptr<LocalPlayer> m_player;
    InventorySlot* m_selected;

    using Frame::setSize;

    Rectf getCellPosition(int index);

public:
    InventoryNode(std::shared_ptr<LocalPlayer> player);

    void draw();
    void update();

    void setEnabled(bool);
};