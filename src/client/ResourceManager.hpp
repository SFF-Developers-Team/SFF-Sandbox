#pragma once

#include "Tilemap.hpp"
#include <filesystem>
#include <unordered_map>

class ResourceManager { 
public:
    static inline ResourceManager& Get() {
        static ResourceManager rm;
        return rm;
    }

    raylib::Texture2D& LoadTexture(std::filesystem::path const& path);
    raylib::Texture2D& GetTexture(std::string const& filename);

    bool LoadTilemap(std::filesystem::path const& path, int tileCountX, int tileCountY);
    Tilemap& GetTilemap(std::string const& filename);
    
private:
    std::unordered_map<std::string, raylib::Texture2D> m_textures;
    std::unordered_map<std::string, Tilemap> m_tilemaps;
};