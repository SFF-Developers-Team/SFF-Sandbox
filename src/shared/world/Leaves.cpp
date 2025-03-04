#include <world/Leaves.hpp>

Leaves::Leaves() : Block(ItemID::LEAVES) {
    setTag(TAG_COLOR, Col3u {0, 228, 48});
}