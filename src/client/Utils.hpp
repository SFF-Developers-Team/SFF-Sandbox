#pragma once

#define TO_CAMERA_POS(camera, pos) {                                                \
        (camera.target.x - camera.offset.x / camera.zoom) + (pos).x / camera.zoom,    \
        (camera.target.y - camera.offset.y / camera.zoom) + (pos).y / camera.zoom     \
}

#define BLOCK_RECT(x, y) {static_cast<float>(x), static_cast<float>(y), 1.f, 1.f}

#define GETTER_SETTER(type, name, member)               \
        type get##name() { return member; };            \
        void set##name(type val) { member = val; }      \