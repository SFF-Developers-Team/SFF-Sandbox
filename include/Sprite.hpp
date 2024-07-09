#pragma once
#include <raylib.h>
#include <map>

class Sprite {
private:
    static inline std::map<int, Sprite*> m_sprites;
    Texture2D tex;

public:
    Sprite(int id);

    Texture2D getTexture();

    int getWidth();
    int getHeight();

    static void loadSprites();
    static void addSprite(int id);
    static Sprite* getSprite(int id);
};
