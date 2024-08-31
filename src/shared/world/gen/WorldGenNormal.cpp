#include <Chunk.hpp>
#include <Block.hpp>
#include <World.hpp>
#include <stdlib.h>
#include <WorldGenNormal.hpp>

WorldGenNormal::WorldGenNormal(World* world, uint64_t seed) : WorldGen(world, seed), m_perlinNoise(seed) {
    srand(seed);
}

std::shared_ptr<Chunk> WorldGenNormal::generateChunk(int32_t position) {
    auto ret = std::make_shared<Chunk>(m_world, position);
    for(auto x = 0u; x < CHUNK_SIZE; x++) {
        for(auto y = 0u; y < m_world->getHeight(); y++) {
            for(auto z = 0u; z < LAYERS; z++) {
                if(y == m_world->getHeight() - 1) {
                    ret->setBlock(x, y, z, Block::Type::BEDROCK);
                    continue;
                }
                
                auto grassLevel = static_cast<int32_t>(m_world->getHeight() * m_perlinNoise.noise2D_01((position * CHUNK_SIZE + x) * 0.01f, z * 0.01f));
                auto stoneLevel = grassLevel + 4 + rand() % 3;

                if(y >= grassLevel + 10 && z == 1 && round(m_perlinNoise.noise2D_01((position * CHUNK_SIZE + x) * 0.2f, y * 0.2f)) == 1) {
                    ret->setBlock(x, y, z, Block::Type::AIR);
                    continue;
                }

                if(y == grassLevel) {
                    ret->setBlock(x, y, z, Block::Type::GRASS);
                    continue;
                }

                if(y > grassLevel && y < stoneLevel) {
                    ret->setBlock(x, y, z, Block::Type::DIRT);
                    continue;
                }

                if(y >= stoneLevel) {
                    ret->setBlock(x, y, z, Block::Type::STONE);
                    continue;
                }
            }
        }
    }

    return ret;
}