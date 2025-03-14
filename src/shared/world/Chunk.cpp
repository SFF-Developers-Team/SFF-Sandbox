#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <world/Block.hpp>
#include <Logger.hpp>
#include <assert.h>
#include <miniz.h>

Chunk::Chunk() : Serializable(CHUNK), m_blocks(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH) {}

bool Chunk::isOutOfBound(BlockPosition pos) {
    return (
        pos.x < (pos.x < 0 ? -CHUNK_WIDTH : 0) || 
        pos.x > (pos.x > 0 ? CHUNK_WIDTH - 1 : 0) || 
        pos.y < 0 || pos.y >= CHUNK_HEIGHT || pos.layer < 0 || 
        pos.layer > CHUNK_DEPTH - 1
    );
}

int Chunk::getIndex(BlockPosition pos) {
    return (pos.layer * CHUNK_WIDTH * CHUNK_HEIGHT) + (pos.y * CHUNK_WIDTH) + abs(pos.x);
}

void Chunk::setBlock(BlockPosition pos, std::shared_ptr<Block> block) {
    if (!isOutOfBound(pos)) {
        if (pos.x < 0) {
            pos.x += CHUNK_WIDTH;
        }

        m_blocks[getIndex(pos)] = block;
    }
}

std::shared_ptr<Block> Chunk::getBlock(BlockPosition pos) {
    if (!isOutOfBound(pos)) {
        if (pos.x < 0) {
            pos.x += CHUNK_WIDTH;
        }

        return m_blocks[getIndex(pos)];
    }

    return nullptr;
}

bool Chunk::isBlockClosed(BlockPosition pos) {
    if (!getBlock({pos.x - 1, pos.y, pos.layer}) || 
        !getBlock({pos.x + 1, pos.y, pos.layer}) || 
        !getBlock({pos.x, pos.y - 1, pos.layer}) || 
        !getBlock({pos.x, pos.y + 1, pos.layer}) || 
        !getBlock({pos.x, pos.y, static_cast<uint8_t>(pos.layer ^ 1)})) {
        return false;
    }

    return true;
}

DataStream Chunk::serialize() {
    auto ret = Serializable::serialize();
    ret.add(countBlocks());

    DataStream blocks;

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (uint8_t layer = 0; layer < CHUNK_DEPTH; layer++) {
                auto block = getBlock({x, y, layer});

                if (block && block->getID() != ItemID::AIR) {
                    blocks.add(ObjectHeader::BLOCK);
                    blocks.add<BlockPosition>({x, y, layer});
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
    
    auto blockCount = stream.get<uint16_t>();

    if (!blockCount) {
        return false;
    }

    // auto wver = m_world->getVersion();
    // uint8_t blockSize = (!wver ? stream.get<uint8_t>() : 0);

    DataStream data;

    // World version 0-1 (raw data)
    // if (wver <= 1) {
    //     data.resize(stream.size() - stream.offset());
    //     std::copy(stream.begin() + stream.offset(), stream.end(), data.begin());
    // }

    // World version 2 (gzip compression)
    // if (wver > 1) {
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
    // }

    std::fill(m_blocks.begin(), m_blocks.end(), nullptr);

    // World version 1
    for (int i = 0; i < blockCount; i++) {
        if (data.get<ObjectHeader>() != ObjectHeader::BLOCK) {
            return false;
        }

        auto pos = data.get<BlockPosition>();    
        
        if(data.get<ObjectHeader>() != ObjectHeader::ITEM) {
            return false;
        }
        
        auto block = Block::create(data.getI<ItemID>());
        block->deserialize(data);
        
        setBlock(pos, block);
    }

#if _DEBUG
    logD("Read {} chunk: {} | block count: {} | data size: {}", (wver > 1 ? "gzip" : "non-gzip"), m_position, blockCount, data.size());
#endif

    return true;
}

uint16_t Chunk::countBlocks() {
    return std::count_if(m_blocks.begin(), m_blocks.end(), [](auto& block) { return block != nullptr; });
}