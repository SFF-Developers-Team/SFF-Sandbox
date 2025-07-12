#pragma once
#include "Box.hpp"
#include "Chunk.hpp"
#include "Types.hpp"
#include <Camera2D.hpp>
#include "worldgen/WorldGenerator.hpp"
#include <RenderTexture.hpp>
#include <string>
#include <vector>
#include <memory>
#include <flat_hash_map.hpp>


class World {
public:
    /**
     * @brief Construct a new World object
     * 
     * @param path where world stored in (must contain world.dat)
     */
    World(std::string const& path, std::unique_ptr<WorldGenerator> worldgen);

    void OnTick();

    BlockID GetBlock(int x, int y, int z);
    void SetBlock(int x, int y, int z, BlockID block);

    std::vector<Box> GetBlocksAround(Box const& box);

    void AddEntity(class Entity* ptr);

#ifdef SANDBOX_CLIENT
    void Draw(raylib::Camera2D& camera, int width, int height);
    void PrepareLightmap(raylib::Camera2D& camera, raylib::RenderTexture2D& lightmap, int width, int height);
#endif

    unsigned long const GetTicks() { return m_ticks; } 
    float const GetDaylightFactor();

    void AddChunk(Vector2i pos, Chunk&& chunk);

    void UpdateLightning();

private:
    ska::flat_hash_map<Vector2i, Chunk, Vector2iHash> m_chunks;
    std::unique_ptr<WorldGenerator> m_worldGenerator;
    std::vector<class Entity*> m_entities;

    unsigned long m_ticks;
};