#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <map>
#include <raylib.h>
#include <TileMap.hpp>


class TextureManager {
private:
    std::map<std::string, std::shared_ptr<TileMap>> m_tilemaps; 
    std::map<std::string, Texture2D> m_textures;
    std::map<std::string, Font> m_fonts;

public:
    static auto get() {
        static auto tm = std::make_shared<TextureManager>();
        return tm;
    }

    bool loadTexture(std::filesystem::path const& filepath);
    bool loadTileMap(std::filesystem::path const& filepath, Vec2i tileSize);
    bool loadFont(std::filesystem::path const& filepath);

    void unloadTexture(std::string const& key);
    void unloadTilemap(std::string const& key);
    void unloadFont(std::string const& key);

    Texture2D& getTexture(std::string const& key);
    Font& getFont(std::string const& key);
    std::shared_ptr<TileMap> getTileMap(std::string const& key);

    bool textureLoaded(std::string const& key);

    float getFontBaseSize(std::string const& fontKey);
};