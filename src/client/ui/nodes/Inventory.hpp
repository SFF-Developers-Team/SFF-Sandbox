#include <ui/nodes/Frame.hpp>
#include <vector>
#include <memory>
#include <entity/Player.hpp>

class Inventory : public Frame {
private:
    std::vector<InventoryItem>& m_inventory;
    int m_selected;

    using Frame::setSize;

    Rectf getCellPosition(int index);

public:
    Inventory(std::vector<InventoryItem>& inventory);

    void draw();
    void update();

    void setEnabled(bool);
};