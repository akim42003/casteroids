#include "asteroid.h"
#include "lib/raylib.h"
#include "lib/raymath.h"
#include <stdlib.h>

void init_asteroid(Asteroid *a, Vector2 position) {
  a->pos = position;

  Vector2 init_vel = {((float)rand() / RAND_MAX - 0.5f) * 150.0f,
                      ((float)rand() / RAND_MAX - 0.5f) * 150.0f};
  a->vel = init_vel;

  a->rotation = 0;
  a->rotationSpeed = ((float)rand() / RAND_MAX - 0.5f) * 0.8f;
  a->scale = 40.0f;
  a->alive = true;

  a->pointCount = 10;
  for (int i = 0; i < a->pointCount; i++) {
    float angle = ((float)i / a->pointCount) * 2 * PI;
    float radius = a->scale * (0.7f + (float)rand() / RAND_MAX * 0.6f);
    a->points[i] = (Vector2){cosf(angle) * radius, sinf(angle) * radius};
  }
};

void update_asteroid(Asteroid *a, float dt) {

  if (!a->alive) {
    return;
  };

  a->pos = Vector2Add(a->pos, Vector2Scale(a->vel, dt));
  a->rotation += a->rotationSpeed * dt;
  int w = GetScreenWidth();
  int h = GetScreenHeight();
  float r = a->scale;

  // wrap around screen
  if (a->pos.x < -r)
    a->pos.x = w + r;
  if (a->pos.x > w + r)
    a->pos.x = -r;
  if (a->pos.y < -r)
    a->pos.y = h + r;
  if (a->pos.y > h + r)
    a->pos.y = -r;
}

void draw_asteroid(const Asteroid *a) {
  if (!a->alive)
    return;

  // draw polygon lines
  for (int i = 0; i < a->pointCount; i++) {
    int next = (i + 1) % a->pointCount;

    // rotate points
    Vector2 p1 = Vector2Rotate(a->points[i], a->rotation);
    Vector2 p2 = Vector2Rotate(a->points[next], a->rotation);

    // translate into world space
    p1 = Vector2Add(a->pos, p1);
    p2 = Vector2Add(a->pos, p2);

    DrawLineV(p1, p2, RAYWHITE);
  }
}
