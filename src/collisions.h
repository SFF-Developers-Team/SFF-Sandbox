#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "raylib.h"

float Collision_ClipX(Rectangle rec1, Rectangle rec2, float xa);
float Collision_ClipY(Rectangle rec1, Rectangle rec2, float ya);

Rectangle ExpandRectangle(Rectangle rect, float x, float y);

#endif