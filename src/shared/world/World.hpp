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
#include <random>

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
    PlayerID m_lastPlayerID = 1;
    uint32_t m_version = WORLD_VERSION;
    uint64_t m_time = 0;
    uint8_t m_randomTickSpeed = 60;
    std::mt19937 m_rng;

public:
    uint32_t const static WORLD_VERSION = 5;

    static std::unique_ptr<World> GenerateWorld(int seed);

    World();

    void onTick();

    void destroyTile(TilePosition pos) { setTile(pos, TileID::AIR); }

    void placeTile(PlayerID id, TilePosition pos);
    void breakTile(PlayerID id, TilePosition pos);
    void stopBreakingTile(PlayerID id);

    void tileChanged(TilePosition position);

    bool isTileClosed(TilePosition pos) const;
    bool isOutOfBound(TilePosition pos) const;
    std::optional<Rectf> getTileHitbox(TilePosition pos);
    std::vector<Hitbox> getHitboxes(Hitbox entityHitbox, int radius = 1);

    bool save() const;
    bool load();

    void setPlayer(PlayerID id, std::shared_ptr<Player> player, std::string const username = "");
    PlayerID addPlayer(std::shared_ptr<Player> player, std::string const username = "");
    std::shared_ptr<Player> getPlayer(PlayerID id) const;
    void unloadPlayer(PlayerID id);
    bool savePlayer(PlayerID id) const;
    bool loadPlayer(std::string const& username = "");
    bool isUsernameAlreadyTaken(std::string const& username) const;

    uint32_t getVersion() const { return m_version; }
    uint64_t getTime() const { return m_time; }

    auto& getPlayers() const { return m_players; }
    auto& getBreakInfo() const { return m_breakInfo; }

    void addStructure(std::shared_ptr<Structure> structure) { m_structures.push_back(structure); }

    inline TileID getTile(TilePosition pos) const {
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
private:
    using CaveMap = std::vector<uint8_t>;

    static float HashNoise(int seed, int x, int y); 
    static CaveMap GenerateCave(int seed, int iterations = 5);
    static void CarveCircle(World& world, TilePosition position, int radius); 
};