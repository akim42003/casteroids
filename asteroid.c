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
  // Random scale for variety (small, medium, large)
  float r = (float)rand() / RAND_MAX;
  if (r < 0.33f) {
    a->scale = 0.3f; // small
  } else if (r < 0.66f) {
    a->scale = 0.6f; // medium
  } else {
    a->scale = 1.0f; // large
  }
  a->alive = true;

  a->pointCount = 10;
  float baseRadius = 40.0f; // Base asteroid radius
  for (int i = 0; i < a->pointCount; i++) {
    float angle = ((float)i / a->pointCount) * 2 * PI;
    float radius = a->scale * baseRadius * (0.7f + (float)rand() / RAND_MAX * 0.6f);
    a->points[i] = (Vector2){cosf(angle) * radius, sinf(angle) * radius};
  }
};

void init_asteroids(Asteroid *asteroids, int max, int screenWidth, int screenHeight) {
  for (int i = 0; i < max; i++) {
    init_asteroid(&asteroids[i],
                  (Vector2){rand() % screenWidth, rand() % screenHeight});
  }
}

void update_asteroid(Asteroid *a, float dt) {

  if (!a->alive) {
    return;
  };

  a->pos = Vector2Add(a->pos, Vector2Scale(a->vel, dt));
  a->rotation += a->rotationSpeed * dt;
  int w = GetScreenWidth();
  int h = GetScreenHeight();
  float r = a->scale * 40.0f; // Use actual radius for wrapping

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

void split_asteroid(Asteroid asteroids[], int max, int destroyedIdx) {
  if (asteroids[destroyedIdx].scale <= 0.4f)
    return;

  int slot1 = -1, slot2 = -1;
  for (int k = 0; k < max; k++) {
    if (!asteroids[k].alive) {
      if (slot1 == -1)
        slot1 = k;
      else if (slot2 == -1) {
        slot2 = k;
        break;
      }
    }
  }

  if (slot1 != -1) {
    init_asteroid(&asteroids[slot1], asteroids[destroyedIdx].pos);
    asteroids[slot1].scale = asteroids[destroyedIdx].scale * 0.5f;
    asteroids[slot1].vel.x += (rand() % 100 - 50);
    asteroids[slot1].vel.y += (rand() % 100 - 50);
  }
  if (slot2 != -1) {
    init_asteroid(&asteroids[slot2], asteroids[destroyedIdx].pos);
    asteroids[slot2].scale = asteroids[destroyedIdx].scale * 0.5f;
    asteroids[slot2].vel.x -= (rand() % 100 - 50);
    asteroids[slot2].vel.y -= (rand() % 100 - 50);
  }
}

int asteroid_points(float scale) {
  if (scale > 0.7f)
    return 20;
  else if (scale > 0.4f)
    return 50;
  else
    return 100;
}
