#include <stdlib.h>
#include "lib/raylib.h"
#include "lib/raymath.h"
#include "asteroid.h"
#include "bullet.h"
#include "ship.h"

typedef enum {
  GAME_STATE_PLAYING,
  GAME_STATE_GAME_OVER
} GameState;

typedef struct {
  int lives;
  int score;
  GameState state;
} GameData;

static void reset_game(GameData *game, Ship *ship, Asteroid asteroids[],
                       int maxAsteroids, Bullet bullets[], int maxBullets,
                       int screenWidth, int screenHeight) {
  game->lives = 3;
  game->score = 0;
  game->state = GAME_STATE_PLAYING;

  ship_init(ship, screenWidth, screenHeight);
  init_asteroids(asteroids, maxAsteroids, screenWidth, screenHeight);
  bullets_init(bullets, maxBullets);
}

static bool bullet_asteroid_collision(const Bullet *bullet,
                                      const Asteroid *asteroid) {
  float distance = Vector2Distance(bullet->pos, asteroid->pos);
  return distance < (asteroid->scale * 30.0f + bullet->r);
}

static void draw_ui(const GameData *game, int screenWidth, int screenHeight) {
  DrawText(TextFormat("Lives: %d", game->lives), 10, 10, 20, WHITE);
  DrawText(TextFormat("Score: %d", game->score), screenWidth - 120, 10, 20,
           WHITE);

  if (game->state == GAME_STATE_GAME_OVER) {
    DrawText("GAME OVER", screenWidth / 2 - 90, screenHeight / 2 - 40, 30, RED);
    DrawText(TextFormat("Final Score: %d", game->score),
             screenWidth / 2 - 80, screenHeight / 2, 20, WHITE);
    DrawText("Press R to Restart", screenWidth / 2 - 100,
             screenHeight / 2 + 40, 20, WHITE);
  }
}

int main() {
  const int screenWidth = 800;
  const int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "Casteroids!");
  SetTargetFPS(60);

  Ship ship = {0};
#define MAX_BULLETS 256
  Bullet bullets[MAX_BULLETS] = {0};
#define MAX_ASTEROIDS 12
  Asteroid asteroids[MAX_ASTEROIDS] = {0};

  static Vector2 prevMouse = {0};
  static bool firstFrame = true;

  GameData game = {0};
  reset_game(&game, &ship, asteroids, MAX_ASTEROIDS, bullets, MAX_BULLETS,
             screenWidth, screenHeight);

  while (!WindowShouldClose()) {
    HideCursor();

    float dt = GetFrameTime();
    Vector2 mouse = GetMousePosition();

    if (firstFrame) {
      prevMouse = mouse;
      firstFrame = false;
    }

    if (game.state == GAME_STATE_GAME_OVER) {
      if (IsKeyPressed(KEY_R)) {
        reset_game(&game, &ship, asteroids, MAX_ASTEROIDS, bullets, MAX_BULLETS,
                   screenWidth, screenHeight);
        prevMouse = GetMousePosition();
      }
    }

    if (game.state == GAME_STATE_PLAYING) {
      if (!ship.alive) {
        ship.respawnTimer -= dt;
        if (ship.respawnTimer <= 0) {
          ship.alive = true;
          ship.invulnerableTimer = 2.0f;
          prevMouse = GetMousePosition();
        }
      }

      if (ship.invulnerableTimer > 0) {
        ship.invulnerableTimer -= dt;
      }

      if (ship.alive) {
        ship_update(&ship, mouse, prevMouse);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          bullet_spawn(bullets, MAX_BULLETS, ship_apex(&ship),
                       ship_angle(&ship));
        }

        if (ship.invulnerableTimer <= 0) {
          for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (asteroids[i].alive &&
                ship_asteroid_collision(&ship, &asteroids[i])) {
              ship.alive = false;
              game.lives--;

              if (game.lives <= 0) {
                game.state = GAME_STATE_GAME_OVER;
              } else {
                ship.respawnTimer = 2.0f;
              }
              break;
            }
          }
        }
      }

      bullets_update(bullets, MAX_BULLETS, dt, screenWidth, screenHeight);

      for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].alive)
          continue;

        for (int j = 0; j < MAX_ASTEROIDS; j++) {
          if (asteroids[j].alive &&
              bullet_asteroid_collision(&bullets[i], &asteroids[j])) {
            bullets[i].alive = false;
            game.score += asteroid_points(asteroids[j].scale);
            split_asteroid(asteroids, MAX_ASTEROIDS, j);
            asteroids[j].alive = false;
            break;
          }
        }
      }
    }

    prevMouse = mouse;

    BeginDrawing();
    ClearBackground(BLACK);

    if (game.state == GAME_STATE_PLAYING || game.state == GAME_STATE_GAME_OVER) {
      if (ship.alive) {
        if (ship.invulnerableTimer <= 0 ||
            ((int)(ship.invulnerableTimer * 10) % 2 == 0)) {
          ship_draw(&ship);
        }
      }

      bullets_draw(bullets, MAX_BULLETS);

      for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (game.state == GAME_STATE_PLAYING) {
          update_asteroid(&asteroids[i], dt);
        }
        draw_asteroid(&asteroids[i]);
      }

      draw_ui(&game, screenWidth, screenHeight);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
