#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <world/Block.hpp>
#include <Logger.hpp>
#include <assert.h>
#include <miniz.h>

Chunk::Chunk(std::shared_ptr<World> world, ChunkPosition pos) : Serializable(CHUNK), m_world(world), m_position(pos) {
    m_blocks.resize(CHUNK_WIDTH * world->getHeight() * CHUNK_DEPTH);
}

void Chunk::onTick() {
    auto x = rand() % CHUNK_WIDTH;
    auto y = rand() % m_world->getHeight();
    auto z = rand() % 2;
    auto block = getBlock(x, y, z);

    if(block) {
        block->onRandomTick(m_world.get(), {x, y, z});
    }
}

bool Chunk::isOutOfBound(int x, int y, uint8_t layer) {
    bool result = (x < (x < 0 ? -CHUNK_WIDTH : 0) || x > (x > 0 ? CHUNK_WIDTH - 1 : 0) || y < 0 || y >= m_world->getHeight() || layer < 0 || layer > CHUNK_DEPTH - 1);

    return result;
}

int Chunk::getIndex(int x, int y, uint8_t layer) {
    return (layer * CHUNK_WIDTH * m_world->getHeight()) + (y * CHUNK_WIDTH) + abs(x);
}

void Chunk::setBlock(int x, int y, uint8_t layer, std::shared_ptr<Block> block) {
    if (!isOutOfBound(x, y, layer)) {
        m_blocks[getIndex((x < 0 ? CHUNK_WIDTH + x : x), y, layer)] = block;
    }
}

void Chunk::setBlock(int x, int y, uint8_t layer, ItemID type) {
    setBlock(x, y, layer, (type == ItemID::AIR ? nullptr : Block::create(type)));
}

std::shared_ptr<Block> Chunk::getBlock(int x, int y, uint8_t layer) {
    if (isOutOfBound(x, y, layer)) {
        return nullptr;
    }

    return m_blocks[getIndex((x < 0 ? CHUNK_WIDTH + x : x), y, layer)];
}

bool Chunk::isBlockClosed(int x, int y, uint8_t layer) {
    bool ret = (
        getBlock(x - 1, y, layer) && 
        getBlock(x + 1, y, layer) && 
        getBlock(x, y - 1, layer) && 
        getBlock(x, y + 1, layer)
    );

    return ret;
}

DataStream Chunk::serialize() {
    auto ret = Serializable::serialize();

    ret.add(m_position);
    ret.add(countBlocks());

    DataStream blocks;

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < m_world->getHeight(); y++) {
            for (int layer = 0; layer < CHUNK_DEPTH; layer++) {
                auto block = getBlock(x, y, layer);

                if (block && block->getID() != ItemID::AIR) {
                    blocks.add(ObjectHeader::BLOCK);
                    blocks.add(x);
                    blocks.add(y);
                    blocks.add(layer);
                    blocks.add(block->serialize());
                }
            }
        }
    }

    uLongf compressedSize = compressBound(blocks.size());
    ByteVector compressed(compressedSize);

    ret.add(static_cast<uint32_t>(compressedSize));

    auto res = compress(compressed.data(), &compressedSize, blocks.data(), blocks.size());

    if (res != Z_OK) {
        logE("Error while compressing chunk! {}", zError(res));
        ret.clear();

        return ret;
    }

    compressed.resize(compressedSize);
    ret.add(compressed);

    return ret;
}

bool Chunk::deserialize(DataStream& stream) {
    if(!Serializable::deserialize(stream)) return false;

    m_position = stream.get<ChunkPosition>();
    auto blockCount = stream.get<uint16_t>();

    if (!blockCount) {
        return false;
    }

    auto wver = m_world->getVersion();
    uint8_t blockSize = (!wver ? stream.get<uint8_t>() : 0);

    DataStream data;

    // World version 0-1 (raw data)
    if (wver <= 1) {
        data.resize(stream.size() - stream.offset());
        std::copy(stream.begin() + stream.offset(), stream.end(), data.begin());
    }

    // World version 2 (gzip compression)
    if (wver > 1) {
        auto size = stream.get<uint32_t>();
        auto compressed = stream.getN(size);
        uLongf decompressedSize = 1024 * 1024;

        data.resize(decompressedSize);

        auto res = uncompress(data.data(), &decompressedSize, compressed.data(), compressed.size());

        if (res != Z_OK) {
            logE("Error while decompress! {}", zError(res));
            return 0;
        }

        data.resize(decompressedSize);
    }

    std::fill(m_blocks.begin(), m_blocks.end(), nullptr);

    // World version 1
    for (int i = 0; i < blockCount; i++) {
        if (data.get<ObjectHeader>() != ObjectHeader::BLOCK) {
            return false;
        }

        auto x = data.get<int>();
        auto y = data.get<int>();
        auto l = data.get<int>();    
        
        
        if(data.get<ObjectHeader>() != ObjectHeader::ITEM) {
            return false;
        }
        
        auto block = Block::create(data.getI<ItemID>());
        block->deserialize(data);
        
        setBlock(x - m_position * CHUNK_WIDTH, y, l, block);
    }

#if _DEBUG
    logD("Read {} chunk: {} | block count: {} | data size: {}", (wver > 1 ? "gzip" : "non-gzip"), m_position, blockCount, data.size());
#endif

    return true;
}

uint16_t Chunk::countBlocks() {
    return std::count_if(m_blocks.begin(), m_blocks.end(), [](auto& block) { return block != nullptr; });
}