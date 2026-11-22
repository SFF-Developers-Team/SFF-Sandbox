#pragma once

#define TILE_RECT(x, y) {static_cast<float>(x), static_cast<float>(y), 1.f, 1.f}

#define TO_BORDER_COLOR(col) { col /= 2; col.a = 255; }