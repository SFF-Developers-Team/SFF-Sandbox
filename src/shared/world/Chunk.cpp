#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <world/Block.hpp>
#include <Logger.hpp>
#include <assert.h>
#include <miniz.h>

Chunk::Chunk(std::shared_ptr<World> world, Chunk::Position pos) : m_world(world), m_position(pos) {
    m_header = Header::CHUNK;
    m_blocks.resize(CHUNK_WIDTH * world->getHeight() * LAYERS);
}

bool Chunk::isOutOfBound(int x, int y, uint8_t layer) {
    bool result = (x < (x < 0 ? -CHUNK_WIDTH : 0) || x > (x > 0 ? CHUNK_WIDTH - 1 : 0) || y < 0 || y >= m_world->getHeight() || layer < 0 || layer > LAYERS - 1);

    return result;
}

int Chunk::getIndex(int x, int y, uint8_t layer) {
    return (layer * CHUNK_WIDTH * m_world->getHeight()) + (y * CHUNK_WIDTH) + abs(x);
}

void Chunk::setBlock(int x, int y, uint8_t layer, std::shared_ptr<Block> block) {
    if (!isOutOfBound(x, y, layer)) {
        if(block != nullptr) {
            block->setPos(m_position * CHUNK_WIDTH + x, y, layer);
        }
        
        m_blocks[getIndex((x < 0 ? CHUNK_WIDTH + x : x), y, layer)] = block;
    }
}

void Chunk::setBlock(int x, int y, uint8_t layer, BlockID type) {
    setBlock(x, y, layer, (type == BlockID::AIR ? nullptr : std::make_shared<Block>(type)));
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

ByteVector Chunk::serialize() {
    SerializedObject::serialize();
    uint16_t blockCount = countBlocks();

    SerializedObject obj;

    add(m_position);
    add(blockCount);

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < m_world->getHeight(); y++) {
            for (int layer = 0; layer < LAYERS; layer++) {
                auto block = getBlock(x, y, layer);

                if (block && block->getID() != BlockID::AIR) {
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

    if (res != Z_OK) {
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

    if (!blockCount) {
        return 0;
    }

    auto wver = m_world->getVersion();
    uint8_t blockSize = (!wver ? get<uint8_t>() : 0);

    SerializedObject data;

    // World version 0-1 (raw data)
    if (wver <= 1) {
        data.add(ByteVector(begin() + m_offset, end()));
        data.reset();
    }

    // World version 2 (gzip compression)
    if (wver > 1) {
        auto size = get<uint32_t>();
        auto compressed = getN(size);
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
        auto bsize = (blockSize > 0) ? blockSize : data.get<uint16_t>();
        assert(bsize > 0);

        auto header = data.getI<Header>();

        if (header == Header::BLOCK) {
            auto bbytes = data.getN(bsize);
            auto block = std::make_shared<Block>();
            block->deserialize(bbytes);

            auto pos = block->getPos();
            auto layer = block->getLayer();

            pos.x -= m_position * CHUNK_WIDTH;

            setBlock(pos.x, pos.y, layer, block);
        }
    }

#if _DEBUG
    logD("Read {} chunk: {} | block count: {} | data size: {}", (wver > 1 ? "gzip" : "non-gzip"), m_position, blockCount, data.size());
#endif

    return m_offset;
}

uint16_t Chunk::countBlocks() {
    return std::count_if(m_blocks.begin(), m_blocks.end(), [](auto& block) { return block != nullptr; });
}