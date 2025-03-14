#include <world/gen/WorldGenNormal.hpp>
#include <world/Chunk.hpp>
#include <world/Block.hpp>
#include <world/World.hpp>
#include <stdlib.h>

struct Ore {
    ItemID type;
    int minHeight;
    int veinSize;
    float density;
};

std::vector<Ore> ores = {
    {ItemID::COAL_ORE, 0, 6, 0.025f},
    {ItemID::IRON_ORE_BLOCK, 256, 6, 0.012f},
    {ItemID::GOLD_ORE_BLOCK, 464, 4, 0.01f},
    {ItemID::DIAMOND_ORE, 488, 2, 0.01f}
};

WorldGenNormal::WorldGenNormal(uint64_t seed) : WorldGen(seed), m_perlinNoise(seed) {
    m_type = NORMAL;
}

std::shared_ptr<Chunk> WorldGenNormal::generateChunk(Vec2i pos) {
    m_random = std::mt19937(m_seed * pos.x);
    std::uniform_real_distribution<> rdis(0.f, 1.f);
    std::uniform_int_distribution<> idis(0, 99);

    auto ret = std::make_shared<Chunk>();
    int lastTree = 0;

    // generating surface
    for (int x = 0u; x < CHUNK_WIDTH; x++) {
        for (uint8_t z = 0u; z < CHUNK_DEPTH; z++) {
            int grassLevel = 64 + 128 * m_perlinNoise.noise2D_01((pos.x * CHUNK_WIDTH + x) * 0.01f, z * 0.01f);
            int stoneLevel = grassLevel + 4 + m_random() % 4;

            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                int wy = pos.y * CHUNK_HEIGHT + y; 

                if (wy == 512) {
                    generateBlock(ret, {x, y, z}, BEDROCK);
                    continue;
                }

                if (wy == grassLevel) {
                    generateBlock(ret, {x, y, z}, GRASS);
                    continue;
                }

                if (wy > grassLevel && wy < stoneLevel) {
                    generateBlock(ret, {x, y, z}, DIRT);
                    continue;
                }

                if (wy >= stoneLevel) {
                    generateBlock(ret, {x, y, z}, STONE);
                    continue;
                }
            }

            // if(z == 0 && idis(m_random) < 10 && x - lastTree > 2) {
            //     m_world->postGenerateTree({pos.x * CHUNK_WIDTH + x, grassLevel, idis(m_random) % 3 + 5});
            //     lastTree = x;
            // }
        }
    }

    // generate ores
    for(auto const& ore : ores) {
        for (auto x = 0u; x < CHUNK_WIDTH; x++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                int wy = pos.y * CHUNK_HEIGHT + y;

                if(wy >= ore.minHeight && rdis(m_random) < ore.density) {
                    for(int i = 0; i < ore.veinSize; i++) {
                        int veinX = x + (m_random() % 3 - 1);
                        int veinY = y + (m_random() % 3 - 1);
                        auto block = ret->getBlock({veinX, veinY, 1});

                        if(ret->isOutOfBound({veinX, veinY, 1}) || block == nullptr) {
                            continue;
                        }

                        if(block->getID() == ItemID::STONE) {
                            generateBlock(ret, {veinX, veinY, 1}, ore.type);
                        }
                    }
                }
            }
        }
    }

    // generate caves
    for (int x = 0u; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            float caveNoise = m_perlinNoise.noise2D_01((pos.x * CHUNK_WIDTH + x) * 0.1f, (pos.y * CHUNK_HEIGHT + y) * 0.1f);

            if(caveNoise > 0.6f) {
                ret->setBlock({x, y, 1}, nullptr);
            }
        }
    }

    return ret;
}