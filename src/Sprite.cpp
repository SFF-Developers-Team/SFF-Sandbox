#include <Sprite.hpp>
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
    grass = new Sprite(1);
    dirt = new Sprite(2);
    stone = new Sprite(3);
}