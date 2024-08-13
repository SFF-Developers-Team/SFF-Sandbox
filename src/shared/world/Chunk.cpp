#include <Chunk.hpp>
#include <World.hpp>
#include <Logger.hpp>
#include <Block.hpp>

Chunk::Chunk(World* world, int pos) : m_world(world), m_position(pos) {
    m_header = Header::CHUNK;
    m_blocks.reserve(CHUNK_WIDTH * world->getHeight() * LAYERS);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < world->getHeight(); y++) {
            setBlock(x, y, 0, Block::BlockType::AIR);
            setBlock(x, y, 1, Block::BlockType::AIR);
        }
    }
}

Chunk::Chunk(World* world) : m_world(world) {
    m_header = Header::CHUNK;
    m_blocks.reserve(CHUNK_WIDTH * world->getHeight() * LAYERS);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < world->getHeight(); y++) {
            setBlock(x, y, 0, Block::BlockType::AIR);
            setBlock(x, y, 1, Block::BlockType::AIR);
        }
    }
}

Chunk::~Chunk() {}

bool Chunk::isOutOfBound(int x, int y, uint8_t layer) {
    return (x < 0 || x > CHUNK_WIDTH || y < 0 || y >= m_world->getHeight() || layer < 0 || layer > LAYERS - 1);
}

int Chunk::getIndex(int x, int y, uint8_t layer) {
    return (layer * CHUNK_WIDTH * m_world->getHeight()) + (y * CHUNK_WIDTH) + x;
}

void Chunk::setBlock(int x, int y, uint8_t layer, std::unique_ptr<Block> block) {
    if(!block || isOutOfBound(x, y, layer)) return;
    
    if(m_blocks[getIndex(x, y, layer)]) m_blocks[getIndex(x, y, layer)].release();

    block->setPosition(m_position * CHUNK_WIDTH + x, y, layer);
    m_blocks[getIndex(x, y, layer)] = std::move(block);
}

void Chunk::setBlock(Vec2i pos, uint8_t layer, std::unique_ptr<Block> block) {
    setBlock(pos.x, pos.y, layer, std::move(block));
}

void Chunk::setBlock(int x, int y, uint8_t layer, Block::BlockType type) {
    setBlock(x, y, layer, std::make_unique<Block>(type));
}

void Chunk::setBlock(Vec2i pos, uint8_t layer, Block::BlockType type) {
    setBlock(pos.x, pos.y, layer, type);
}

Block* Chunk::getBlock(int x, int y, uint8_t layer) {
    if(isOutOfBound(x, y, layer)) return nullptr;

    return m_blocks[getIndex(x, y, layer)].get();
}

Block* Chunk::getBlock(Vec2i pos, uint8_t layer) {
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

void Chunk::generate() {
    for(int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < m_world->getHeight(); y++) {\
            for(int layer = 0; layer < LAYERS; layer++) {
                setBlock(x, y, layer, m_world->getGenerator()->generateBlock(m_position * CHUNK_SIZE + x, y, layer));
            }
        }
    }
}

ByteVector& Chunk::serialize() {
    SerializedObject::serialize();
    unsigned short blockCount = 0;

    for(int i = 0; i < m_blocks.capacity(); i++) {
        if(m_blocks[i] && m_blocks[i]->getType() != Block::BlockType::AIR) blockCount++;
    }

    addBytes(m_position);
    addBytes(blockCount);
    addBytes((unsigned char)Block::getSize());

    for(int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < m_world->getHeight(); y++) {
            for(int layer = 0; layer < LAYERS; layer++) {
                auto block = getBlock(x, y, layer);

                if(block && block->getType() != Block::BlockType::AIR) {
                    addBytes(block->serialize());
                }
            }
        }
    }

    return m_bytes;
}

int Chunk::deserialize(ByteVector& bytes) {
    SerializedObject::deserialize(bytes);

    m_position = getBytes<ChunkPosition>();
    unsigned short blockCount = getBytes<unsigned short>();
    unsigned char blockSize = getBytes<unsigned char>();

    if(!blockCount) {
        return m_offset;
    }

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < m_world->getHeight(); y++) {
            setBlock(x, y, 0, Block::BlockType::AIR);
            setBlock(x, y, 1, Block::BlockType::AIR);
        }
    }

    logD("chunk {} block count: {}, block size: {}", m_position, blockCount, blockSize);

    for(int i = 0; i < blockCount; i++) {
        auto blockBytes = getBytes((size_t)blockSize);
        auto block = std::make_unique<Block>(Block::BlockType::AIR);
        block->deserialize(blockBytes);

        auto pos = block->getPosition();
        auto layer = block->getLayer();

        pos.x -= m_position * CHUNK_WIDTH;

        setBlock(pos, layer, std::move(block));
    }

    return m_offset;
}

uint32_t Chunk::getBlockCount() {
    uint32_t ret = 0;

    for(auto& block : m_blocks) {
        if(block && block->getType() != Block::BlockType::AIR) ret++;
    }

    return ret;
}