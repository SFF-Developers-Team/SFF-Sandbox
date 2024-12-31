#pragma once

#define TO_CAMERA_POS(camera, pos) {                                                \
        (camera.target.x - camera.offset.x / camera.zoom) + (pos).x / camera.zoom,    \
        (camera.target.y - camera.offset.y / camera.zoom) + (pos).y / camera.zoom     \
}