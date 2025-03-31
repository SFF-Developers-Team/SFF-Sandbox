#include <world/TreeStructure.hpp>
#include <world/Block.hpp>

TreeStructure::TreeStructure(Vec2i pos, int height) : Structure(pos, {5, height}) {
    for (auto y = 2; y < height; y++) {
        setBlock({m_size.x / 2, y, 0}, Block::create(OAK_LOG));
    }

    for (int x = -m_size.x / 2; x < m_size.x / 2; x++) {
        for (int y = -m_size.x / 2; y < m_size.x / 2; y++) {
            if (x * x + y * y <= m_size.x) {
                if (x != m_size.x / 2) {
                    setBlock({m_size.x / 2 + x, y, 0}, Block::create(LEAVES));
                }

                setBlock({m_size.x / 2 + x, y, 1}, Block::create(LEAVES));
            }
        }
    }
}