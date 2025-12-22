#ifndef SHIP_H_
#define SHIP_H_

#include "asteroid.h"
#include "lib/raylib.h"

typedef struct {
  Vector2 v1;
  Vector2 v2;
  Vector2 v3;
  float angle;
  bool alive;
  float respawnTimer;
  float invulnerableTimer;
} Ship;

void ship_init(Ship *ship, int screenWidth, int screenHeight);
void ship_update(Ship *ship, Vector2 mouse, Vector2 prevMouse);
void ship_draw(const Ship *ship);
Vector2 ship_apex(const Ship *ship);
float ship_angle(const Ship *ship);
Vector2 ship_center(const Ship *ship);
bool ship_asteroid_collision(const Ship *ship, const Asteroid *asteroid);

#endif
