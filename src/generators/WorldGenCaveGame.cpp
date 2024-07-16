#include <WorldGenCaveGame.hpp>
#include <World.hpp>
#include <Block.hpp>

WorldGenCaveGame::WorldGenCaveGame(World* world) : WorldGen(world) {
    m_perlin = siv::PerlinNoise(1);
}

Block *WorldGenCaveGame::generateBlock(int x, int y) {
    Block::BlockType type = Block::BlockType::AIR;

    if(y >= m_world->getHeight() - 1) return new Block(Block::BlockType::STONE, x, y);

    const float coef = 0.1f;
    if(round(m_perlin.noise2D_01(x * coef, y * coef)) == 0.0f) return nullptr;

    int grassLevel = (int)(m_world->getHeight() * 2 / 3);

    if(y == grassLevel) {
        type = Block::BlockType::GRASS;
    } else if(y >= grassLevel) {
        type = Block::BlockType::STONE;
    } else {
        return nullptr;
    } 

    return new Block(type, x, y);
}