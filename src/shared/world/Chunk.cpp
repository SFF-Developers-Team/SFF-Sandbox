#include <Chunk.hpp>
#include <World.hpp>
#include <Logger.hpp>
#include <Block.hpp>
#include <assert.h>

Chunk::Chunk(std::shared_ptr<World> world, ChunkPos pos) : m_world(world), m_position(pos) {
    m_header = Header::CHUNK;
    m_blocks.resize(CHUNK_WIDTH * world->getHeight() * LAYERS);

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for(int y = 0; y < world->getHeight(); y++) {
            setBlock(x, y, 0, Block::ID::AIR);
            setBlock(x, y, 1, Block::ID::AIR);
        }
    }
}

bool Chunk::isOutOfBound(int x, int y, uint8_t layer) {
    bool result = (
        x < (x < 0 ? -CHUNK_WIDTH : 0) || 
        x > ( x > 0 ? CHUNK_WIDTH : 0) || 
        y < 0 || y >= m_world->getHeight() || 
        layer < 0 || layer > LAYERS - 1
    );

    return result;
}

int Chunk::getIndex(int x, int y, uint8_t layer) {
    return (layer * CHUNK_WIDTH * m_world->getHeight()) + (y * CHUNK_WIDTH) + abs(x);
}

void Chunk::setBlock(int x, int y, uint8_t layer, std::shared_ptr<Block> block) {
    assert(block != nullptr);

    if(!isOutOfBound(x, y, layer)) {
        block->setPos(m_position * CHUNK_WIDTH + x, y, layer);
        m_blocks[getIndex((x < 0 ? CHUNK_WIDTH + x : x), y, layer)] = block;
    }
}

void Chunk::setBlock(Vec2i pos, uint8_t layer, std::shared_ptr<Block> block) {
    setBlock(pos.x, pos.y, layer, block);
}

void Chunk::setBlock(int x, int y, uint8_t layer, Block::ID type) {
    setBlock(x, y, layer, std::make_shared<Block>(type));
}

void Chunk::setBlock(Vec2i pos, uint8_t layer, Block::ID type) {
    setBlock(pos.x, pos.y, layer, type);
}

std::shared_ptr<Block> Chunk::getBlock(int x, int y, uint8_t layer) {
    if(isOutOfBound(x, y, layer)) {
        return nullptr;
    }

    return m_blocks[getIndex((x < 0 ? CHUNK_WIDTH + x : x), y, layer)];
}

std::shared_ptr<Block> Chunk::getBlock(Vec2i pos, uint8_t layer) {
    return getBlock(pos.x, pos.y, layer);
}

int Chunk::getHeight() {
    return m_world->getHeight();
}

bool Chunk::isBlockClosed(int x, int y, uint8_t layer) {
    return getBlock(x - 1, y, layer) && 
           getBlock(x + 1, y, layer) && 
           getBlock(x, y - 1, layer) && 
           getBlock(x, y + 1, layer);
}

bool Chunk::isBlockClosed(Vec2i pos, uint8_t layer) {
    return isBlockClosed(pos.x, pos.y, layer);
}

ByteVector& Chunk::serialize() {
    SerializedObject::serialize();
    uint16_t blockCount = countBlocks();

    addBytes(m_position);
    addBytes(blockCount);

    for(int x = 0; x < CHUNK_WIDTH; x++) {
        for(int y = 0; y < m_world->getHeight(); y++) {
            for(int layer = 0; layer < LAYERS; layer++) {
                auto block = getBlock(x, y, layer);

                if(block && block->getID() != Block::ID::AIR) {
                    addBytes(block->serialize());
                }
            }
        }
    }

    return m_bytes;
}

size_t Chunk::deserialize(ByteVector& bytes) {
    SerializedObject::deserialize(bytes);

    m_position = getBytes<ChunkPos>();

    auto blockCount = getBytes<uint16_t>();
    logD("chunk {} block count: {}", m_position, blockCount);

    if(!blockCount) {
        return m_offset;
    }

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for(int y = 0; y < m_world->getHeight(); y++) {
            setBlock(x, y, 0, Block::ID::AIR);
            setBlock(x, y, 1, Block::ID::AIR);
        }
    }

    for(int i = 0; i < blockCount; i++) {
        auto header = getBytes<Header>(true);
        if(header == Header::BLOCK) {
            std::size_t size = getBytes<uint16_t>();
            assert(("Null size block!", size > 0));

            auto blockBytes = getBytes(size);
            auto block = std::make_shared<Block>();
            block->deserialize(blockBytes);

            auto pos = block->getPos();
            auto layer = block->getLayer();

            pos.x -= m_position * CHUNK_WIDTH;

            setBlock(pos, layer, block);
        }
    }

    return m_offset;
}

uint16_t Chunk::countBlocks() {
    uint16_t ret = 0;

    for(auto i = 0; i < m_blocks.capacity(); i++) {
        if(m_blocks[i] && m_blocks[i]->getID() != Block::ID::AIR) ret++;
    }

    return ret;
}