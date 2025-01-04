#include <Chunk.hpp>
#include <World.hpp>
#include <Logger.hpp>
#include <Block.hpp>
#include <assert.h>
#include <zlib.h>

Chunk::Chunk(std::shared_ptr<World> world, Chunk::Position pos) : m_world(world), m_position(pos) {
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

ByteVector Chunk::serialize() {
    SerializedObject::serialize();
    uint16_t blockCount = countBlocks();

    SerializedObject obj;

    add(m_position);
    add(blockCount);

    for(int x = 0; x < CHUNK_WIDTH; x++) {
        for(int y = 0; y < m_world->getHeight(); y++) {
            for(int layer = 0; layer < LAYERS; layer++) {
                auto block = getBlock(x, y, layer);

                if(block && block->getID() != Block::ID::AIR) {
                    auto bytes = block->serialize();
                    obj.add<uint16_t>(bytes.size());
                    obj.add(bytes);
                }
            }
        }
    }

    uLongf compressedSize = compressBound(obj.size());
    ByteVector compressed(compressedSize);
    auto res = compress(compressed.data(), &compressedSize, obj.data(), obj.size());

    if(res != Z_OK) {
        logE("Error while compressing chunk! {}", zError(res));
        clear();

        return bytes();
    }

    compressed.resize(compressedSize);

    add(static_cast<uint32_t>(compressedSize));
    add(compressed);

    return bytes();
}

size_t Chunk::deserialize(ByteVector const& bytes) {
    SerializedObject::deserialize(bytes);

    m_position = get<Chunk::Position>();

    auto blockCount = get<uint16_t>();
    logD("chunk {} block count: {}", m_position, blockCount);

    if(!blockCount) return m_offset;

    auto wver = m_world->getVersion();
    uint8_t blockSize = (!wver ? get<uint8_t>() : 0);

    SerializedObject data;

    // World version 0-1
    if(wver <= 1) {
        data.add(ByteVector(begin() + m_offset, end()));
        data.reset();
    }

    // World version 2
    if(wver > 1) {
        logD("Decompressing gzip chunk...");
        auto size = get<uint32_t>();
        auto compressed = getN(size);
        uLongf decompressedSize = 1024 * 1024;
    
        data.resize(decompressedSize);

        auto res = uncompress(data.data(), &decompressedSize, compressed.data(), compressed.size());

        if(res != Z_OK) {
            logE("Error while decompress! {}", zError(res));
            return 0;
        }

        logD("Success!");
        data.resize(decompressedSize);
    }

    logD("chunk data size {}", data.size());

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for(int y = 0; y < m_world->getHeight(); y++) {
            setBlock(x, y, 0, Block::ID::AIR);
            setBlock(x, y, 1, Block::ID::AIR);
        }
    }

    // World version 1
    for(int i = 0; i < blockCount; i++) {
        auto bsize = (blockSize > 0) ? blockSize : data.get<uint16_t>();
        assert(("Null size block!", bsize > 0));
        
        auto header = data.getI<Header>();

        if(header == Header::BLOCK) {
            auto bbytes = data.getN(bsize);
            auto block = std::make_shared<Block>();
            block->deserialize(bbytes);

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