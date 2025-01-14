#include <world/gen/WorldGenNormal.hpp>
#include <world/Chunk.hpp>
#include <world/Block.hpp>
#include <world/World.hpp>
#include <stdlib.h>

WorldGenNormal::WorldGenNormal(std::shared_ptr<World> world, uint64_t seed) : WorldGen(world, seed), m_perlinNoise(seed) {
    srand(seed);
    m_type = NORMAL;
}

std::shared_ptr<Chunk> WorldGenNormal::generateChunk(int32_t position) {
    auto ret = std::make_shared<Chunk>(m_world, position);
    for (auto x = 0u; x < CHUNK_WIDTH; x++) {
        for (auto y = 0u; y < m_world->getHeight(); y++) {
            for (auto z = 0u; z < LAYERS; z++) {
                if (y == m_world->getHeight() - 1) {
                    ret->setBlock(x, y, z, Block::ID::BEDROCK);
                    continue;
                }

                auto grassLevel = static_cast<int32_t>(m_world->getHeight() * m_perlinNoise.noise2D_01(fabs(INT_MAX / 2 + position * CHUNK_WIDTH + x) * 0.01f, z * 0.01f));
                auto stoneLevel = grassLevel + 4 + rand() % 3;

                if (y >= grassLevel + 10 && z == 1 && round(m_perlinNoise.noise2D_01(fabs(INT_MAX / 2 + position * CHUNK_WIDTH + x) * 0.2f, y * 0.2f)) == 1) {
                    ret->setBlock(x, y, z, Block::ID::AIR);
                    continue;
                }

                if (y == grassLevel) {
                    ret->setBlock(x, y, z, Block::ID::GRASS);
                    continue;
                }

                if (y > grassLevel && y < stoneLevel) {
                    ret->setBlock(x, y, z, Block::ID::DIRT);
                    continue;
                }

                if (y >= stoneLevel) {
                    ret->setBlock(x, y, z, Block::ID::STONE);
                    continue;
                }
            }
        }
    }

    return ret;
}