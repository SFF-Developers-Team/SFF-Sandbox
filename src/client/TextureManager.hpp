#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <map>
#include <raylib.h>

class TextureManager {
private:
    std::map<std::string, Texture2D> m_textures;

public:
    static auto get() {
        static auto tm = std::make_shared<TextureManager>();
        return tm;
    }

    bool loadTexture(std::filesystem::path const& filepath);
    void unloadTexture(std::string const& key);
    Texture2D& getTexture(std::string const& key);
    bool textureLoaded(std::string const& key);
};