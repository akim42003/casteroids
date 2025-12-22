#ifndef BULLET_H
#define BULLET_H

#include "lib/raylib.h"

typedef struct {
  Vector2 pos;
  Vector2 vel;
  float r;
  bool alive;
} Bullet;

void bullets_init(Bullet *bullets, int max);
void bullet_spawn(Bullet *bullets, int max, Vector2 pos, float angle);
void bullets_update(Bullet *bullets, int max, float dt, int screenWidth, int screenHeight);
void bullets_draw(const Bullet *bullets, int max);

#endif
