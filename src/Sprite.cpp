#include <Sprite.hpp>
#include <Block.hpp>
#include <string>

Sprite::Sprite(int id) {
    this->tex = LoadTexture(std::string("assets/" + std::to_string(id) + ".png").c_str());
}

Texture2D Sprite::getTexture() {
    return this->tex;
}

int Sprite::getWidth() {
    return this->tex.width;
}

int Sprite::getHeight() {
    return this->tex.height;
}

void Sprite::loadSprites() {
    Sprite::addSprite(GRASS);
    Sprite::addSprite(DIRT);
    Sprite::addSprite(STONE);
}

void Sprite::addSprite(int id) {
    m_sprites[id] = new Sprite(id);
}

Sprite* Sprite::getSprite(int id) {
    return m_sprites[id];
}