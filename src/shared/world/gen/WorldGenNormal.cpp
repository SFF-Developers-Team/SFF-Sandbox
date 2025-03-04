#include <world/gen/WorldGenNormal.hpp>
#include <world/Chunk.hpp>
#include <world/Block.hpp>
#include <world/World.hpp>
#include <stdlib.h>

struct Ore {
    ItemID type;
    int maxHeight;
    int veinSize;
    float density;
};

std::vector<Ore> ores = {
    {ItemID::COAL_ORE, 128, 6, 0.025f},
    {ItemID::IRON_ORE, 64, 6, 0.012f},
    {ItemID::GOLD_ORE, 16, 4, 0.01f},
    {ItemID::DIAMOND_ORE, 12, 2, 0.01f}
};

WorldGenNormal::WorldGenNormal(std::shared_ptr<World> world, uint64_t seed) : WorldGen(world, seed), m_perlinNoise(seed) {
    m_type = NORMAL;
}

std::shared_ptr<Chunk> WorldGenNormal::generateChunk(int32_t position) {
    m_random = std::mt19937(m_seed * position);
    std::uniform_real_distribution<> rdis(0.f, 1.f);
    std::uniform_int_distribution<> idis(0, 99);

    auto ret = std::make_shared<Chunk>(m_world, position);
    int lastTree = 0;

    // generating surface
    for (int x = 0u; x < CHUNK_WIDTH; x++) {
        for (int z = 0u; z < LAYERS; z++) {
            int grassLevel = m_world->getHeight() * m_perlinNoise.noise2D_01(fabs(INT_MAX / 2 + position * CHUNK_WIDTH + x) * 0.01f, z * 0.01f);
            int stoneLevel = grassLevel + 4 + m_random() % 3;

            for (auto y = 0u; y < m_world->getHeight(); y++) {
                if (y == m_world->getHeight() - 1) {
                    ret->setBlock(x, y, z, ItemID::BEDROCK);
                    continue;
                }

                if (y == grassLevel) {
                    ret->setBlock(x, y, z, ItemID::GRASS);
                    continue;
                }

                if (y > grassLevel && y < stoneLevel) {
                    ret->setBlock(x, y, z, ItemID::DIRT);
                    continue;
                }

                if (y >= stoneLevel) {
                    ret->setBlock(x, y, z, ItemID::STONE);
                    continue;
                }
            }

            if(z == 0 && idis(m_random) < 10 && x - lastTree > 2) {
                m_world->postGenerateTree({position * CHUNK_WIDTH + x, grassLevel, idis(m_random) % 3 + 5});
                lastTree = x;
            }
        }
    }

    // generate ores
    for(auto const& ore : ores) {
        for (auto x = 0u; x < CHUNK_WIDTH; x++) {
            for (auto y = m_world->getHeight() - ore.maxHeight; y < m_world->getHeight() - 1; y++) {
                if(rdis(m_random) < ore.density) {
                    for(int i = 0; i < ore.veinSize; i++) {
                        int veinX = x + (m_random() % 3 - 1);
                        int veinY = y + (m_random() % 3 - 1);
                        auto block = ret->getBlock(veinX, veinY, 1);

                        if(ret->isOutOfBound(veinX, veinY, 1) || block == nullptr) {
                            continue;
                        }

                        if(block->getID() == ItemID::STONE) {
                            ret->setBlock(veinX, veinY, 1, ore.type);
                        }
                    }
                }
            }
        }
    }

    // generate caves
    for (auto x = 0u; x < CHUNK_WIDTH; x++) {
        for (auto y = 0u; y < m_world->getHeight() - 1; y++) {
            float caveNoise = m_perlinNoise.noise2D_01(fabs(0x7FFFFFFF + position * CHUNK_WIDTH + x) * 0.1f, y * 0.1f);

            if(caveNoise > 0.6f) {
                ret->setBlock(x, y, 1, nullptr);
            }
        }
    }

    return ret;
}