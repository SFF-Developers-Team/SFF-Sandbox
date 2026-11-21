#pragma once
#include "Types.hpp"
#include <entity/Player.hpp>
#include <world/Block.hpp>
#include <entity/Hitbox.hpp>

#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <map>
#include <array>
#include <unordered_map>
#include <optional>

class LocalPlayer;
class WorldGen;
class Structure;

struct BreakInfo {
    BlockPosition pos;
    uint64_t time;
    float progress;
};

#define WORLD_SIZE 512
#define WORLD_LAYERS 2

class World {
private:
    std::array<BlockID, WORLD_SIZE*WORLD_SIZE*WORLD_LAYERS> m_blocks;

protected:
    std::map<PlayerID, std::shared_ptr<Player>> m_players;
    std::vector<std::shared_ptr<Structure>> m_structures;
    std::unordered_map<PlayerID, BreakInfo> m_breakInfo;
    std::shared_ptr<WorldGen> m_worldGen;
    std::filesystem::path m_saveDir;
    PlayerID m_lastPlayerID;
    uint32_t m_version;
    uint64_t m_time;
    uint8_t m_randomTickSpeed;

public:
    uint32_t const static WORLD_VERSION = 5;

    World(std::filesystem::path const& saveDir);

    void generate();
    void onTick();

    void placeBlock(BlockPosition pos, BlockID blockId) { if (getBlock(pos) != BlockID::AIR) setBlock(pos, blockId); }
    void destroyBlock(BlockPosition pos) { setBlock(pos, BlockID::AIR); }

    void placeBlock(PlayerID id, BlockPosition pos);
    void breakBlock(PlayerID id, BlockPosition pos);
    void stopBreakingBlock(PlayerID id);

    bool isBlockClosed(BlockPosition pos);
    bool isOutOfBound(BlockPosition pos);
    std::optional<Rectf> getBlockHitbox(BlockPosition pos);
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

    void addStructure(std::shared_ptr<Structure> structure) { m_structures.push_back(structure); }

    inline BlockID getBlock(BlockPosition pos) {
        if (isOutOfBound(pos)) {
            return BlockID::AIR;
        }

        return m_blocks[pos.layer * (WORLD_SIZE * WORLD_SIZE) + pos.y * WORLD_SIZE + pos.x];
    }

    inline void setBlock(BlockPosition pos, BlockID block) {
        if (isOutOfBound(pos)) {
            return;
        }

        m_blocks[pos.layer * (WORLD_SIZE * WORLD_SIZE) + pos.y * WORLD_SIZE + pos.x] = block;
    }
};