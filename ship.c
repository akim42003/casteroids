#include "ship.h"
#include "lib/raymath.h"
#include <math.h>

static void ship_set_vertices(Ship *ship, Vector2 pos) {
  const float halfBase = 12.0f;
  const float height = 30.0f;

  Vector2 baseL = (Vector2){-halfBase, height};
  Vector2 baseR = (Vector2){halfBase, height};
  Vector2 rotL = Vector2Rotate(baseL, ship->angle + PI / 2.0f);
  Vector2 rotR = Vector2Rotate(baseR, ship->angle + PI / 2.0f);

  ship->v1 = pos;
  ship->v2 = Vector2Add(pos, rotL);
  ship->v3 = Vector2Add(pos, rotR);
}

void ship_init(Ship *ship, int screenWidth, int screenHeight) {
  Vector2 center = {screenWidth / 2.0f, screenHeight / 2.0f};

  ship->angle = -PI / 2.0f;
  ship->alive = true;
  ship->respawnTimer = 0.0f;
  ship->invulnerableTimer = 0.0f;
  ship_set_vertices(ship, center);
}

void ship_update(Ship *ship, Vector2 mouse, Vector2 prevMouse) {
  Vector2 delta = Vector2Subtract(mouse, prevMouse);
  float speed2 = delta.x * delta.x + delta.y * delta.y;

  if (speed2 > 0.1f) {
    ship->angle = atan2f(delta.y, delta.x);
  }

  ship_set_vertices(ship, mouse);
}

void ship_draw(const Ship *ship) {
  DrawTriangle(ship->v1, ship->v2, ship->v3, WHITE);
}

Vector2 ship_apex(const Ship *ship) {
  return ship->v1;
}

float ship_angle(const Ship *ship) {
  return ship->angle;
}

Vector2 ship_center(const Ship *ship) {
  Vector2 center = {(ship->v1.x + ship->v2.x + ship->v3.x) / 3.0f,
                    (ship->v1.y + ship->v2.y + ship->v3.y) / 3.0f};
  return center;
}

bool ship_asteroid_collision(const Ship *ship, const Asteroid *asteroid) {
  Vector2 center = ship_center(ship);
  float distance = Vector2Distance(center, asteroid->pos);
  return distance < (asteroid->scale * 30.0f + 15.0f);
}
