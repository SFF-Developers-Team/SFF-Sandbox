#pragma once
#include "Types.hpp"
#include <entity/Player.hpp>
#include <world/Block.hpp>
#include <world/Chunk.hpp>
#include <entity/Hitbox.hpp>

#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <map>
#include <unordered_map>

#define WORLDVER 4

class LocalPlayer;
class WorldGen;

struct BreakInfo {
    BlockPosition pos;
    uint64_t time;
    float progress;
};

class World {
protected:
    std::map<PlayerID, std::shared_ptr<Player>> m_players;
    std::unordered_map<Vec2i, std::shared_ptr<Chunk>, Vec2iHash> m_chunks;
    std::unordered_map<PlayerID, BreakInfo> m_breakInfo;
    std::shared_ptr<WorldGen> m_worldGen;
    std::filesystem::path m_saveDir;
    PlayerID m_lastPlayerID;
    uint32_t m_version;
    uint64_t m_time;
    uint8_t m_randomTickSpeed;

public:
    uint32_t const WORLD_VERSION = WORLDVER;

    World(std::filesystem::path const& saveDir);

    void generate();
    void onTick();

    void placeBlock(BlockPosition pos, std::shared_ptr<Block> block) { if (!getBlock(pos)) setBlock(pos, block); }
    void destroyBlock(BlockPosition pos) { setBlock(pos, nullptr); }

    void placeBlock(PlayerID m_id, BlockPosition pos);
    void breakBlock(PlayerID id, BlockPosition pos);
    void stopBreakingBlock(PlayerID id);

    bool isBlockClosed(BlockPosition pos);
    Rectf getBlockHitbox(BlockPosition pos);
    std::shared_ptr<Block> getBlock(BlockPosition pos);
    void setBlock(BlockPosition pos, std::shared_ptr<Block> block);
    bool isOutOfBound(BlockPosition pos);

    void unloadChunk(Vec2i pos) { m_chunks.erase(pos); }
    virtual void addChunk(Vec2i pos, std::shared_ptr<Chunk> chunk) { m_chunks[pos] = chunk; }

    std::shared_ptr<Chunk> getChunk(Vec2i position);
    bool saveChunk(Vec2i pos);
    bool loadChunk(Vec2i pos);

    std::vector<Hitbox> getHitboxes(Hitbox entityHitbox, int radius = 1);

    bool save();
    bool load();

    void setPlayer(PlayerID id, std::shared_ptr<Player> player, std::string const username = "");
    PlayerID addPlayer(std::shared_ptr<Player> player, std::string const username = "");
    std::shared_ptr<Player> getPlayer(PlayerID id);
    void unloadPlayer(PlayerID id);
    bool savePlayer(PlayerID id);
    bool loadPlayer(std::string const& username = "");
    bool isUsernameAlreadyTaken(std::string const& username);

    void setGenerator(std::shared_ptr<WorldGen> generator) { m_worldGen = generator; }

    uint32_t getVersion() { return m_version; }
    uint64_t getTime() { return m_time; }
    auto& getGenerator() { return m_worldGen; }
    auto& getPlayers() { return m_players; }
    auto& getChunks() { return m_chunks; }
    
    void generateChunk(Vec2i pos);
};