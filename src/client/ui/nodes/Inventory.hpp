#include <ui/nodes/Frame.hpp>
#include <vector>
#include <memory>

class Block;

class Inventory : public Frame {
private:
    std::vector<std::shared_ptr<Block>>& m_inventory;
    int m_selected;
    using Frame::setSize;

    Rectf getCellPosition(int index);

public:
    Inventory(std::vector<std::shared_ptr<Block>>& inventory);

    void draw();
    void update();

    void setEnabled(bool);
};