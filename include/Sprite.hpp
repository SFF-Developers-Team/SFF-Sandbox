#pragma once
#include <raylib.h>

class Sprite {
private:
    Texture2D tex;

public:
    static inline Sprite* grass;
    static inline Sprite* dirt;
    static inline Sprite* stone;
    
    Sprite(int id);

    Texture2D getTexture();

    int getWidth();
    int getHeight();

    static void loadSprites();
};
