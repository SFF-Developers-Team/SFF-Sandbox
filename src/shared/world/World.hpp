#pragma once
#include <vector>
#include <string>
#include <map>
#include <Hitbox.hpp>
#include <SerializedObject.hpp>
#include <Block.hpp>
#include <Chunk.hpp>
#include <SimplePlayer.hpp>
#include <memory>

class Player;
class WorldGen;

class World : public SerializedObject {
private:
    std::map<ChunkPos, std::shared_ptr<Chunk>> m_chunks;
    std::map<PlayerID, std::shared_ptr<SimplePlayer>> m_players;
    std::shared_ptr<WorldGen> m_worldGen;
    std::string m_worldName;

    uint32_t m_width = 0;
    uint32_t m_height = 0;
    PlayerID m_lastPlayerID = 1;

public:
    const uint32_t WORLD_VERSION = 1;

    World(uint32_t width, uint32_t height, std::string const& worldName);
    World(std::string const& worldName);
    ~World();

    void generate();
    void onTick();

    void placeBlock(int32_t x, int32_t y, uint8_t layer, std::shared_ptr<Block> block);
    void destroyBlock(int32_t x, int32_t y, uint8_t layer);
    Rectf getBlockHitbox(int32_t x, int32_t y);
    bool isBlockClosed(int32_t x, int32_t y, uint8_t layer);
    void setBlock(int32_t x, int32_t y, uint8_t layer, std::shared_ptr<Block> block);
    std::shared_ptr<Block> getBlock(int32_t x, int32_t y, uint8_t layer);

    void unloadChunk(std::shared_ptr<Chunk> chunk);
    void unloadChunk(ChunkPos pos);
    void addChunk(std::shared_ptr<Chunk> chunk);
    std::shared_ptr<Chunk> getChunk(int32_t position);

    std::vector<Hitbox> getHitboxes(Hitbox entityHitbox);

    bool save();
    bool load();

    ByteVector& serialize() override;
    size_t deserialize(ByteVector& bytes) override;

    void addPlayer(PlayerID id, std::shared_ptr<SimplePlayer> player, std::string const username = "");
    PlayerID addPlayer(std::shared_ptr<SimplePlayer> player, std::string const username = "");
    std::shared_ptr<SimplePlayer> getPlayer(PlayerID id);
    void unloadPlayer(PlayerID id);
    bool isUsernameAlreadyTaken(std::string const& username);
    bool isOutOfBound(int32_t x, int32_t y, uint8_t layer);

    inline ChunkPos convertXtoChunkPosition(int32_t x) { return floorf(static_cast<float>(x) / CHUNK_WIDTH); }

    void setGenerator(std::shared_ptr<WorldGen> generator) { m_worldGen = generator; }

    auto getWidth() { return m_width; }
    auto getHeight() { return m_height; }
    auto getGenerator() { return m_worldGen; }
    auto& getPlayers() { return m_players; }
    auto& getChunks() { return m_chunks; }
};  