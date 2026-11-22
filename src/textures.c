#include "textures.h"
#include "defines.h"
#include "utils.h"
#include <string.h>

struct TextureEntry* texturesHead;
struct TextureEntry* texturesTail;

int LoadTextureEntry(struct TextureEntry* tex) {
    static char path[MAX_PATH];

    strcpy(path, "resources/");
    strcat(path, tex->name);

    tex->texture = LoadTexture(path);

    if (!tex->texture.id) {
        TraceLog(LOG_ERROR, "TEXTURE: Failed to load texture %s", tex->name);
        return -1;
    } 

    LINKEDLIST_APPEND(tex, texturesHead, texturesTail);

    return 0;
}

Texture2D* FindTexture(const char* name) {
    struct TextureEntry* entry;

	for (entry = texturesHead; entry; entry = entry->next) {
		if (!TextIsEqual(name, entry->name)) continue;

		return &entry->texture;
	}

    TraceLog(LOG_WARNING, "TEXTURE: Can't find texture %s", name);

	return NULL;
}