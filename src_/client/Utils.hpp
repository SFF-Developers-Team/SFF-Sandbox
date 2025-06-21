#pragma once

#define TO_CAMERA_POS(camera, pos) {                                                \
        (camera.target.x - camera.offset.x / camera.zoom) + (pos).x / camera.zoom,    \
        (camera.target.y - camera.offset.y / camera.zoom) + (pos).y / camera.zoom     \
}

#define BLOCK_RECT(x, y) {static_cast<float>(x), static_cast<float>(y), 1.f, 1.f}

#define TO_BORDER_COLOR(col) { col /= 2; col.a = 255; }