#pragma once
#include <entity/SimplePlayer.hpp>
#include <world/Block.hpp>
#include <world/Chunk.hpp>
#include <SerializedObject.hpp>
#include <entity/Hitbox.hpp>

#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <map>

#define WORLDVER 4

class Player;
class WorldGen;

class World : public SerializedObject {
private:
    std::map<ChunkPosition, std::shared_ptr<Chunk>> m_chunks;
    std::map<PlayerID, std::shared_ptr<SimplePlayer>> m_players;
    std::vector<TreeStructure> m_postGenTrees;
    std::shared_ptr<WorldGen> m_worldGen;
    std::filesystem::path m_saveDir;
    PlayerID m_lastPlayerID;
    uint32_t m_version;
    uint32_t m_height;
    uint64_t m_time;

public:
    uint32_t const WORLD_VERSION = WORLDVER;

    World(uint32_t height, std::filesystem::path const& saveDir);
    World(std::filesystem::path const& saveDir);

    void generate();
    void onTick();

    void placeBlock(int32_t x, int32_t y, uint8_t layer, std::shared_ptr<Block> block);
    void destroyBlock(int32_t x, int32_t y, uint8_t layer);
    Rectf getBlockHitbox(int32_t x, int32_t y);
    bool isBlockClosed(int32_t x, int32_t y, uint8_t layer);
    void setBlock(int32_t x, int32_t y, uint8_t layer, std::shared_ptr<Block> block, bool natural = false);
    std::shared_ptr<Block> getBlock(int32_t x, int32_t y, uint8_t layer);

    void unloadChunk(ChunkPosition pos);
    void addChunk(std::shared_ptr<Chunk> chunk);
    std::shared_ptr<Chunk> getChunk(ChunkPosition position);
    bool saveChunk(ChunkPosition pos);
    bool loadChunk(ChunkPosition pos);

    std::vector<Hitbox> getHitboxes(Hitbox entityHitbox, int radius = 1);

    bool save();
    bool load();

    ByteVector serialize() override;
    size_t deserialize(ByteVector const& bytes) override;

    void addPlayer(PlayerID id, std::shared_ptr<SimplePlayer> player, std::string const username = "");
    PlayerID addPlayer(std::shared_ptr<SimplePlayer> player, std::string const username = "");
    std::shared_ptr<SimplePlayer> getPlayer(PlayerID id);
    void unloadPlayer(PlayerID id);
    bool savePlayer(PlayerID id);
    bool loadPlayer(std::string const& username = "");
    bool isUsernameAlreadyTaken(std::string const& username);
    bool isOutOfBound(int x, int y, uint8_t layer);

    ChunkPosition xToChunk(int32_t x) { return static_cast<ChunkPosition>(floor(static_cast<float>(x) / CHUNK_WIDTH)); }

    void setGenerator(std::shared_ptr<WorldGen> generator) { m_worldGen = generator; }

    uint32_t getHeight() { return m_height; }
    uint32_t getVersion() { return m_version; }
    uint64_t getTime() { return m_time; }
    auto const& getGenerator() { return m_worldGen; }
    auto const& getPlayers() { return m_players; }
    auto const& getChunks() { return m_chunks; }

    void postGenerateTree(TreeStructure tree) { m_postGenTrees.push_back(tree); }
    void generateChunk(ChunkPosition xPos);
};