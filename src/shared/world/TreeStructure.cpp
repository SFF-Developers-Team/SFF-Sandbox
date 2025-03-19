#include <TreeStructure.hpp>
#include <Block.hpp>

TreeStructure::TreeStructure(int height) : Structure({5, height}) {
    for (auto y = 2; y < height; y++) {
        setBlock({m_size.x / 2, y, 0}, Block::create(OAK_LOG));
    }

    for (int x = 0; x <= 6; ++x) {
        for (int y = 0; y <= 4; ++y) {
            if (x * x + y * y <= 3 * 2) {
                if (y < 2 && x != m_size.x / 2) {
                    setBlock({x, y, 0}, Block::create(LEAVES));
                }

                setBlock({x, y, 1}, Block::create(LEAVES));
            }
        }
    }
}