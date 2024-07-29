#include <Chunk.hpp>
#include <World.hpp>
#include <Logger.hpp>

Chunk::Chunk(World* world, int pos) : m_world(world), m_position(pos) {
    m_header = Header::CHUNK;
    m_blocks.reserve(CHUNK_WIDTH * world->getHeight() * LAYERS);
}

Chunk::Chunk(World* world) : m_world(world) {
    m_header = Header::CHUNK;
    m_blocks.reserve(CHUNK_WIDTH * world->getHeight() * LAYERS);
}

Chunk::~Chunk() {}

bool Chunk::isOutOfBound(int x, int y, int layer) {
    return (x < 0 || x > CHUNK_WIDTH || y < 0 || y > m_world->getHeight() || layer < 0 || layer > LAYERS - 1);
}

int Chunk::getIndex(int x, int y, int layer) {
    return (layer * CHUNK_WIDTH * m_world->getHeight()) + (y * CHUNK_WIDTH) + x;
}

void Chunk::setBlock(int x, int y, int layer, Block* block) {
    if(isOutOfBound(x, y, layer)) return;
    
    if(getBlock(x, y, layer)) {
        delete m_blocks[getIndex(x, y, layer)];
    }

    if(block) block->setPosition(x, y, layer);
    
    m_blocks[getIndex(x, y, layer)] = block;

}

void Chunk::setBlock(Vec2i pos, int layer, Block* block) {
    setBlock(pos.x, pos.y, layer, block);
}

void Chunk::setBlock(int x, int y, int layer, Block::BlockType type) {
    setBlock(x, y, layer, new Block(type));
}

void Chunk::setBlock(Vec2i pos, int layer, Block::BlockType type) {
    setBlock(pos.x, pos.y, layer, type);
}

Block* Chunk::getBlock(int x, int y, int layer) {
    if(isOutOfBound(x, y, layer)) return nullptr;

    return m_blocks[getIndex(x, y, layer)];
}

Block* Chunk::getBlock(Vec2i pos, int layer) {
    return getBlock(pos.x, pos.y, layer);
}

int Chunk::getHeight() {
    return m_world->getHeight();
}

bool Chunk::isBlockClosed(int x, int y, int layer) {
    return getBlock(x - 1, y, layer) && 
           getBlock(x + 1, y, layer) && 
           getBlock(x, y - 1, layer) && 
           getBlock(x, y + 1, layer);
}

bool Chunk::isBlockClosed(Vec2i pos, int layer) {
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
        if(m_blocks[i] != nullptr) blockCount++;
    }

    addBytes(m_position);
    addBytes(blockCount);
    addBytes((unsigned char)Block::getSize());

    for(int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < m_world->getHeight(); y++) {
            for(int layer = 0; layer < LAYERS; layer++) {
                auto block = getBlock(x, y, layer);

                if(block != nullptr) {
                    addBytes(block->serialize());
                }
            }
        }
    }

    return m_bytes;
}

int Chunk::deserialize(ByteVector& bytes) {
    SerializedObject::deserialize(bytes);
    
    for(int i = 0; i < m_blocks.capacity(); i++) {
        m_blocks[i] = nullptr;
    }

    m_position = getBytes<int>();
    unsigned short blockCount = getBytes<unsigned short>();
    unsigned char blockSize = getBytes<unsigned char>();

    if(!blockCount) {
        return m_offset;
    }

    logD("chunk {} block count: {}, block size: {}", m_position, blockCount, blockSize);

    for(int i = 0; i < blockCount; i++) {
        auto block = new Block(Block::BlockType::AIR);

        ByteVector blockBytes(m_bytes.begin() + m_offset, m_bytes.begin() + m_offset + blockSize);

        m_offset += block->deserialize(blockBytes);
        setBlock(block->getPosition(), block->getLayer(), block);
    }

    return m_offset;
}