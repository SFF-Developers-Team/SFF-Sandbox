#pragma once
#include <vector>
#include <string>
#include <map>
#include <Hitbox.hpp>
#include <SerializedObject.hpp>
#include <Block.hpp>
#include <SimplePlayer.hpp>
#include <memory>

#define CHUNK_SIZE 16

class Player;
class Chunk;
class WorldGen;

class World : public SerializedObject {
private:
    std::vector<std::shared_ptr<Chunk>> m_chunks;
    std::map<PlayerID, std::shared_ptr<SimplePlayer>> m_players;
    std::shared_ptr<WorldGen> m_worldGen;

    uint32_t m_width;
    uint32_t m_height;
    PlayerID m_lastPlayerID = 1;

public:
    World(int width, int height);
    ~World();

    void generate(std::shared_ptr<WorldGen> generator);
    void onTick();
    bool isBlockClosed(int x, int y, uint8_t layer);

    void setBlock(int x, int y, uint8_t layer, std::shared_ptr<Block> block);
    std::shared_ptr<Block> getBlock(int x, int y, uint8_t layer);

    void unloadChunk(std::shared_ptr<Chunk> chunk);
    void setChunk(std::shared_ptr<Chunk> chunk);
    std::shared_ptr<Chunk> getChunk(int32_t position);
    Rectf getBlockHitbox(int x, int y);

    std::vector<Hitbox> getHitboxes(Hitbox entityHitbox);

    void placeBlock(int x, int y, uint8_t layer, enum Block::Type id);
    void destroyBlock(int x, int y, uint8_t layer);

    bool save();
    bool load();

    ByteVector& serialize() override;
    size_t deserialize(ByteVector& bytes) override;

    void addPlayer(PlayerID id, std::shared_ptr<SimplePlayer> player, std::string const username = "");
    PlayerID addPlayer(std::shared_ptr<SimplePlayer> player, std::string const username = "");
    std::shared_ptr<SimplePlayer> getPlayer(PlayerID id);
    void unloadPlayer(PlayerID id);
    bool isUsernameAlreadyTaken(std::string const& username);
    bool isOutOfBound(int x, int y, uint8_t layer);

    auto getWidth() { return m_width; }
    auto getHeight() { return m_height; }
    auto getGenerator() { return m_worldGen; }
    auto& getPlayers() { return m_players; }
    auto& getChunks() { return m_chunks; }
};  