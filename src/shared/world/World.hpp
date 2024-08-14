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
    std::vector<Chunk*> m_chunks;
    std::map<PlayerID, std::shared_ptr<SimplePlayer>> m_players;
    WorldGen* m_WorldGen;

    uint32_t m_width;
    uint32_t m_height;
    PlayerID m_lastPlayerID = 1;

public:
    World(int width, int height);
    ~World();

    void generate(WorldGen* generator);
    void onTick();
    bool isBlockClosed(int x, int y, uint8_t layer);

    void setBlock(int x, int y, uint8_t layer, std::unique_ptr<Block> block);
    Block* getBlock(int x, int y, uint8_t layer);

    void unloadChunk(Chunk* chunk);
    void setChunk(Chunk* chunk);
    Chunk* getChunk(int position);
    Rectf getBlockHitbox(int x, int y);

    std::vector<Hitbox> getHitboxes(Hitbox entityHitbox);

    void placeBlock(int x, int y, uint8_t layer, enum Block::BlockType id);
    void destroyBlock(int x, int y, uint8_t layer);

    bool save();
    bool load();

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);

    void addPlayer(PlayerID id, std::shared_ptr<SimplePlayer> player);
    PlayerID addPlayer(std::shared_ptr<SimplePlayer> player);
    std::shared_ptr<SimplePlayer> getPlayer(PlayerID id);
    void unloadPlayer(PlayerID id);
    bool isUsernameAlreadyTaken(std::string const& username);
    bool isOutOfBound(int x, int y, uint8_t layer);

    auto getWidth() { return m_width; }
    auto getHeight() { return m_height; }
    auto getGenerator() { return m_WorldGen; }
    auto& getPlayers() { return m_players; }
    auto& getChunks() { return m_chunks; }
};  