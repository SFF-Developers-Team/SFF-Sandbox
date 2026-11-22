#pragma once
#include "Types.hpp"
#include <entity/Player.hpp>
#include <world/Tile.hpp>
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
    TilePosition pos;
    uint64_t time;
    float progress;
};

#define WORLD_SIZE 512
#define WORLD_LAYERS 2

class World {
private:
    std::array<TileID, WORLD_SIZE*WORLD_SIZE*WORLD_LAYERS> m_tiles;

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

    void placeTile(TilePosition pos, TileID tile) { if (getTile(pos) != TileID::AIR) setTile(pos, tile); }
    void destroyTile(TilePosition pos) { setTile(pos, TileID::AIR); }

    void placeTile(PlayerID id, TilePosition pos);
    void breakTile(PlayerID id, TilePosition pos);
    void stopBreakingTile(PlayerID id);

    bool isTileClosed(TilePosition pos);
    bool isOutOfBound(TilePosition pos);
    std::optional<Rectf> getTileHitbox(TilePosition pos);
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

    inline TileID getTile(TilePosition pos) {
        if (isOutOfBound(pos)) {
            return TileID::AIR;
        }

        return m_tiles[pos.layer * (WORLD_SIZE * WORLD_SIZE) + pos.y * WORLD_SIZE + pos.x];
    }

    inline void setTile(TilePosition pos, TileID tile) {
        if (isOutOfBound(pos)) {
            return;
        }

        m_tiles[pos.layer * (WORLD_SIZE * WORLD_SIZE) + pos.y * WORLD_SIZE + pos.x] = tile;
    }
};