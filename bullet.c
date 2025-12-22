#include "bullet.h"
#include "lib/raymath.h"
#include <math.h>

void bullets_init(Bullet *bullets, int max) {
  for (int i = 0; i < max; i++) {
    bullets[i].alive = false;
  }
}

void bullet_spawn(Bullet *bullets, int max, Vector2 pos, float angle) {
  Vector2 dir = (Vector2){cosf(angle), sinf(angle)};
  if (Vector2LengthSqr(dir) < 1e-6f)
    dir = (Vector2){0, -1};

  for (int i = 0; i < max; i++) {
    if (!bullets[i].alive) {
      bullets[i].pos = pos;
      bullets[i].vel = Vector2Scale(dir, 500.0f);
      bullets[i].r = 5.0f;
      bullets[i].alive = true;
      break;
    }
  }
}

void bullets_update(Bullet *bullets, int max, float dt, int screenWidth, int screenHeight) {
  for (int i = 0; i < max; i++) {
    if (!bullets[i].alive)
      continue;

    bullets[i].pos = Vector2Add(bullets[i].pos, Vector2Scale(bullets[i].vel, dt));

    if (bullets[i].pos.x < -20 || bullets[i].pos.x > screenWidth + 20 ||
        bullets[i].pos.y < -20 || bullets[i].pos.y > screenHeight + 20) {
      bullets[i].alive = false;
    }
  }
}

void bullets_draw(const Bullet *bullets, int max) {
  for (int i = 0; i < max; i++) {
    if (bullets[i].alive) {
      DrawCircleV(bullets[i].pos, bullets[i].r, SKYBLUE);
    }
  }
}
