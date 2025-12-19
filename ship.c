#include "asteroid.h"
#include "lib/raylib.h"
#include "lib/raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Game states
typedef enum {
  GAME_STATE_PLAYING,
  GAME_STATE_GAME_OVER
} GameState;

// Bullet struct
typedef struct {
  Vector2 pos;
  Vector2 vel;
  float r;
  bool alive;
} Bullet;

// Game data
typedef struct {
  int lives;
  int score;
  GameState state;
  bool shipAlive;
  float shipRespawnTimer;
  float shipInvulnerableTimer;
} GameData;

// Collision detection helpers
bool CheckShipAsteroidCollision(Vector2 v1, Vector2 v2, Vector2 v3, const Asteroid *asteroid) {
  Vector2 shipCenter = {(v1.x + v2.x + v3.x) / 3.0f, (v1.y + v2.y + v3.y) / 3.0f};
  float distance = Vector2Distance(shipCenter, asteroid->pos);
  return distance < (asteroid->scale * 30.0f + 15.0f);
}

bool CheckBulletAsteroidCollision(const Bullet *bullet, const Asteroid *asteroid) {
  float distance = Vector2Distance(bullet->pos, asteroid->pos);
  return distance < (asteroid->scale * 30.0f + bullet->r);
}

// Game management
void ResetGame(GameData *game, Asteroid asteroids[], int maxAsteroids, Bullet bullets[], int maxBullets) {
  game->lives = 3;
  game->score = 0;
  game->state = GAME_STATE_PLAYING;
  game->shipAlive = true;
  game->shipRespawnTimer = 0.0f;
  game->shipInvulnerableTimer = 0.0f;
  
  for (int i = 0; i < maxAsteroids; i++) {
    init_asteroid(&asteroids[i], (Vector2){rand() % 800, rand() % 450});
  }
  
  for (int i = 0; i < maxBullets; i++) {
    bullets[i].alive = false;
  }
}

void SpawnBullet(Bullet bullets[], int maxBullets, Vector2 pos, float angle) {
  Vector2 dir = (Vector2){cosf(angle), sinf(angle)};
  if (Vector2LengthSqr(dir) < 1e-6f)
    dir = (Vector2){0, -1};

  for (int i = 0; i < maxBullets; i++) {
    if (!bullets[i].alive) {
      bullets[i].pos = pos;
      bullets[i].vel = Vector2Scale(dir, 500.0f);
      bullets[i].r = 5.0f;
      bullets[i].alive = true;
      break;
    }
  }
}

void SplitAsteroid(Asteroid asteroids[], int maxAsteroids, int destroyedIdx) {
  if (asteroids[destroyedIdx].scale <= 0.4f) return;
  
  int slot1 = -1, slot2 = -1;
  for (int k = 0; k < maxAsteroids; k++) {
    if (!asteroids[k].alive) {
      if (slot1 == -1) slot1 = k;
      else if (slot2 == -1) { slot2 = k; break; }
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

int GetAsteroidPoints(float scale) {
  if (scale > 0.7f) return 20;
  else if (scale > 0.4f) return 50;
  else return 100;
}

void UpdateShip(GameData *game, Vector2 *v1, Vector2 *v2, Vector2 *v3, Vector2 mouse, Vector2 prevMouse, float *angle) {
  const float halfBase = 12.0f;
  const float height = 30.0f;
  
  Vector2 delta = Vector2Subtract(mouse, prevMouse);
  float speed2 = delta.x * delta.x + delta.y * delta.y;
  
  if (speed2 > 0.1f) {
    *angle = atan2f(delta.y, delta.x);
  }
  
  Vector2 baseL = (Vector2){-halfBase, height};
  Vector2 baseR = (Vector2){halfBase, height};
  Vector2 rotL = Vector2Rotate(baseL, *angle + PI / 2.0f);
  Vector2 rotR = Vector2Rotate(baseR, *angle + PI / 2.0f);
  
  *v1 = mouse;
  *v2 = Vector2Add(mouse, rotL);
  *v3 = Vector2Add(mouse, rotR);
}

void DrawUI(const GameData *game, int screenWidth, int screenHeight) {
  DrawText(TextFormat("Lives: %d", game->lives), 10, 10, 20, WHITE);
  DrawText(TextFormat("Score: %d", game->score), screenWidth - 120, 10, 20, WHITE);
  
  if (game->state == GAME_STATE_GAME_OVER) {
    DrawText("GAME OVER", screenWidth/2 - 90, screenHeight/2 - 40, 30, RED);
    DrawText(TextFormat("Final Score: %d", game->score), screenWidth/2 - 80, screenHeight/2, 20, WHITE);
    DrawText("Press R to Restart", screenWidth/2 - 100, screenHeight/2 + 40, 20, WHITE);
  }
}

int ship() {
  const int screenWidth = 800;
  const int screenHeight = 800;

  SetTargetFPS(60);

  Vector2 triangleV1 = {screenWidth / 2.0f + 15.0f, 370.0f};
  Vector2 triangleV2 = {screenWidth / 2.0f, 400.0f};
  Vector2 triangleV3 = {screenWidth / 2.0f + 30.0f, 400.0f};
// Bullet pool
#define MAX_BULLETS 256
  Bullet bullets[MAX_BULLETS] = {0};
#define MAX_ASTEROIDS 12
  Asteroid asteroids[MAX_ASTEROIDS];
  // persistent state
  const float halfBase = 12.0f;
  const float height = 30.0f;

  static Vector2 prevMouse = {0};
  static bool firstFrame = true;
  static float angle = -PI / 2.0f; // default: pointing "up"
  
  // Game data
  GameData game = {0};
  ResetGame(&game, asteroids, MAX_ASTEROIDS, bullets, MAX_BULLETS);

  while (!WindowShouldClose()) {
    HideCursor();

    float dt = GetFrameTime();
    Vector2 mouse = GetMousePosition();

    // Initialize on first frame
    if (firstFrame) {
      prevMouse = mouse;
      firstFrame = false;
    }
    
    // Handle game states
    if (game.state == GAME_STATE_GAME_OVER) {
      if (IsKeyPressed(KEY_R)) {
        ResetGame(&game, asteroids, MAX_ASTEROIDS, bullets, MAX_BULLETS);
        prevMouse = GetMousePosition();
      }
    }
    
    // Update game logic only when playing
    if (game.state == GAME_STATE_PLAYING) {
      // Update ship respawn
      if (!game.shipAlive) {
        game.shipRespawnTimer -= dt;
        if (game.shipRespawnTimer <= 0) {
          game.shipAlive = true;
          game.shipInvulnerableTimer = 2.0f; // 2 seconds of invulnerability
          prevMouse = GetMousePosition();
        }
      }
      
      // Update invulnerability timer
      if (game.shipInvulnerableTimer > 0) {
        game.shipInvulnerableTimer -= dt;
      }

      // Ship control (only when alive)
      if (game.shipAlive) {
        UpdateShip(&game, &triangleV1, &triangleV2, &triangleV3, mouse, prevMouse, &angle);

        // spawn bullet
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          SpawnBullet(bullets, MAX_BULLETS, apex, angle);
        }
        
        // Check ship-asteroid collisions (only when not invulnerable)
        if (game.shipInvulnerableTimer <= 0) {
          for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (asteroids[i].alive && CheckShipAsteroidCollision(triangleV1, triangleV2, triangleV3, &asteroids[i])) {
              // Ship hit!
              game.shipAlive = false;
              game.lives--;
              
              if (game.lives <= 0) {
                game.state = GAME_STATE_GAME_OVER;
              } else {
                game.shipRespawnTimer = 2.0f; // 2 second respawn delay
              }
              break;
            }
          }
        }
      }

      // --- Update bullets ---
      for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].alive)
          continue;

        bullets[i].pos =
            Vector2Add(bullets[i].pos, Vector2Scale(bullets[i].vel, dt));

        // kill bullets off-screen
        if (bullets[i].pos.x < -20 || bullets[i].pos.x > screenWidth + 20 ||
            bullets[i].pos.y < -20 || bullets[i].pos.y > screenHeight + 20) {
          bullets[i].alive = false;
          continue;
        }
        
        // Check bullet-asteroid collisions
        for (int j = 0; j < MAX_ASTEROIDS; j++) {
          if (asteroids[j].alive && CheckBulletAsteroidCollision(&bullets[i], &asteroids[j])) {
            bullets[i].alive = false;
            
            game.score += GetAsteroidPoints(asteroids[j].scale);
            SplitAsteroid(asteroids, MAX_ASTEROIDS, j);
            asteroids[j].alive = false;
            break;
          }
        }
      }
    } // end if GAME_STATE_PLAYING

    // Prepare for next frame
    prevMouse = mouse;

    BeginDrawing();
    ClearBackground(BLACK);

    // Draw game
    if (game.state == GAME_STATE_PLAYING || game.state == GAME_STATE_GAME_OVER) {
      // Draw ship (only when alive and visible)
      if (game.shipAlive) {
        // Blink ship when invulnerable
        if (game.shipInvulnerableTimer <= 0 || ((int)(game.shipInvulnerableTimer * 10) % 2 == 0)) {
          DrawTriangle(triangleV1, triangleV2, triangleV3, WHITE);
        }
      }

      // Draw bullets
      for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].alive) {
          DrawCircleV(bullets[i].pos, bullets[i].r, SKYBLUE);
        }
      }
      
      // Update and draw asteroids
      for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (game.state == GAME_STATE_PLAYING) {
          update_asteroid(&asteroids[i], dt);
        }
        draw_asteroid(&asteroids[i]);
      }
      
      // Draw UI
      DrawUI(&game, screenWidth, screenHeight);
    }

    EndDrawing();
  }

  return 0;
}
