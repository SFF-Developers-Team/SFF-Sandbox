#include <World.hpp>
#include <iostream>
#include <Sprite.hpp>

World::World(int width, int height) : m_width(width), m_height(height) {
    m_blocks = new Block[width * height];

    this->generate();
}

void World::generate() {
    std::fill_n(m_blocks, m_width * m_height, AIR);

    for (int x = 0; x < m_width; x++) {
        for(int y = 0; y < m_height; y++){
            int index = x * m_height + y;

            // m_blocks[index] = STONE;

            int grassLevel = (int)(m_height * 2 / 3);

            if(y == grassLevel) {
                m_blocks[index] = GRASS;
                continue;
            }

            if(y > grassLevel && y < grassLevel + 5) {
                m_blocks[index] = DIRT;
                continue;
            }

            if(y >= grassLevel + 5) {
                m_blocks[index] = STONE;
                continue;
            }
        }
    }
}

void World::draw(Vector2 playerPosition, int renderDistance, bool debug) {
    int playerInChunk = (int)(playerPosition.x / 32 / CHUNK_SIZE) % CHUNK_SIZE;

    int minX = fmax((playerInChunk - ceil(renderDistance / 2)) * CHUNK_SIZE, 0);
    int maxX = fmin((playerInChunk + ceil(renderDistance / 2)) * CHUNK_SIZE, m_width);


    for (int x = minX; x < maxX; x++) {
        for(int y = 0; y < m_height; y++) {
            auto block = m_blocks[x * m_height + y];

            switch (block) {
                case GRASS:
                    DrawTexturePro(Sprite::grass->getTexture(), {0, 0, (float)Sprite::grass->getWidth(), (float)Sprite::grass->getHeight()}, {(float)x * 32, (float)y * 32, 32, 32}, {0, 0}, 0, WHITE);
                    break;
                
                case DIRT:
                    DrawTexturePro(Sprite::dirt->getTexture(), {0, 0, (float)Sprite::dirt->getWidth(), (float)Sprite::dirt->getHeight()}, {(float)x * 32, (float)y * 32, 32, 32}, {0, 0}, 0, WHITE);
                    break;
                
                case STONE:
                    DrawTexturePro(Sprite::stone->getTexture(), {0, 0, (float)Sprite::stone->getWidth(), (float)Sprite::stone->getHeight()}, {(float)x * 32, (float)y * 32, 32, 32}, {0, 0}, 0, WHITE);
                    break;

                default:
                    break;
            }
        }

        if(debug) {
            DrawLineV({(float)playerInChunk * CHUNK_SIZE * 32, 0}, {(float)playerInChunk * CHUNK_SIZE * 32, (float)m_height * 32}, YELLOW);
            DrawLineV({(float)playerInChunk * (CHUNK_SIZE * 2) * 32, 0}, {(float)playerInChunk * (CHUNK_SIZE * 2) * 32, (float)m_height * 32}, YELLOW);
        }
    }
}

bool World::isBlockAccesible(int x, int y) {
    return m_blocks[x * m_height + (y + 1)] == AIR || 
           m_blocks[x * m_height + (y - 1)] == AIR || 
           m_blocks[(x + 1) * m_height + y] == AIR || 
           m_blocks[(x - 1) * m_height + y] == AIR;
}

// void World::BlockCheck() {
//     for (int x = 0; x < 256; x++) {
//         for(int y = 0; y < 64; y++) {
//             if(isBlockAccesible(x, y) && worldArr[x][y] != AIR) {
//                 hitboxes.push_back(Vector2 {(float)x, (float)y});
//             }
//         }
//     }
// }