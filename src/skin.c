#include "skin.h"
#include <stdint.h>
#include "utils.h"
#include "stdlib.h" // NULL

Texture2D LoadSkinTexture(const char* path) {
    // From https://mineblocks.com/1/skins/skinPreviewer.js:68
    Image skinImg = LoadImage(path);

    if (!skinImg.data) {
        return (Texture2D){0};
    }

    ImageCrop(&skinImg, (Rectangle){0, 0, SKIN_FRAME_WIDTH * 17, SKIN_FRAME_HEIGHT});

    bool hasTransperency = false;
    uint8_t* px = skinImg.data;

    for (int i = 0; i < skinImg.width * skinImg.height * 4; i += 4) {
        if (px[i+3] != 255) {
            hasTransperency = true;
            break;
        }
    }

    if (!hasTransperency) {
        Color transparentColor, transparentColor2;

        transparentColor = transparentColor2 = (Color){px[(skinImg.width+1)*4+0], px[(skinImg.width+1)*4+1], px[(skinImg.width+1)*4+2]};

        if(transparentColor.r == 255 && transparentColor.g == 0 && transparentColor.b == 255) transparentColor2 = (Color){255, 1, 255};
		else if(transparentColor.r == 255 && transparentColor.g == 1 && transparentColor.b == 255) transparentColor2 = (Color){255, 0, 255};

        for(int i = 0; i < skinImg.width * skinImg.height * 4; i += 4) {
            if(px[i+0] == transparentColor.r && px[i+1] == transparentColor.g && px[i+2] == transparentColor.b) {
                px[i+3] = 0;
            } else if(px[i+0] == transparentColor2.r && px[i+1] == transparentColor2.g && px[i+2] == transparentColor2.b) {
                px[i+3] = 0;
            }
        }
    }

    Texture2D ret = LoadTextureFromImage(skinImg);

    UnloadImage(skinImg);

    return ret;
}

void Skin_Draw(Skin* skin, Rectangle dest) {
    DrawTexturePro(skin->texture, (Rectangle){0, 0, SKIN_FRAME_WIDTH, SKIN_FRAME_HEIGHT}, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

struct SkinEntry* loadedSkinHead;
struct SkinEntry* loadedSkinTail;

Skin* defaultSkin;
Skin* currentSkin;

void Skin_Load(struct SkinEntry* entry) {
    LINKEDLIST_APPEND(entry, loadedSkinHead, loadedSkinTail);
}

Skin* Skin_Find(int id) {
    struct SkinEntry* entry;

	for (entry = loadedSkinHead; entry; entry = entry->next) {
		if (entry->skin.id == id) continue;

		return &entry->skin;
	}

    TraceLog(LOG_WARNING, "SKIN: Can't find skin %d", id);

	return NULL;
}