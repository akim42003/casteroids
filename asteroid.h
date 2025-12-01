#ifndef ASTEROID_H
#define ASTEROID_H

#include "lib/raylib.h"

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float rotation;
    float rotationSpeed;
    float scale;
    bool alive;

    Vector2 points[10]; // polygon vertices in local space
    int pointCount;
} Asteroid;

void init_asteroid(Asteroid *a, Vector2 position);
void update_asteroid(Asteroid *a, float dt);
void draw_asteroid(const Asteroid *a);

#endif
