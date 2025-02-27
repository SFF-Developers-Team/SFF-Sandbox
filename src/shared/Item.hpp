#pragma once
#include <ItemBase.hpp>
#include <Types.hpp>

class Item : public ItemBase {
public:
    static std::string idToString(ItemID id);

    Item(ItemID id);
};