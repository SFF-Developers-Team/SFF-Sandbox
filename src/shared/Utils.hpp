#pragma once

#define INDEX_2D(x, y, w) (y * w + x)
#define INDEX_3D(x, y, z, w, h) ((z * w * h) + (y * w) + x)