#include <stdio.h>
#include <stdlib.h>
#include "lib/raylib.h"
#include "lib/raymath.h"
#include "asteroid.h"
// Bullet struct
typedef struct {
     Vector2 pos;
     Vector2 vel;
     float   r;
     bool    alive;
} Bullet;

int ship() {
     	const int screenWidth  = 800;
     	const int screenHeight = 800;

     	SetTargetFPS(60);

     	Vector2 triangleV1 = { screenWidth/2.0f + 15.0f, 370.0f };
     	Vector2 triangleV2 = { screenWidth/2.0f , 400.0f };
     	Vector2 triangleV3 = { screenWidth/2.0f + 30.0f, 400.0f };

     	// Bullet pool
     	#define MAX_BULLETS 256
     	Bullet bullets[MAX_BULLETS] = {0};
	#define MAX_ASTEROIDS 12
	Asteroid asteroids[MAX_ASTEROIDS];
     // persistent state
	const float halfBase = 12.0f;
     	const float height   = 30.0f;

     	static Vector2 prevMouse  = {0};
     	static bool    firstFrame = true;
     	static float   angle      = -PI/2.0f; // default: pointing "up"

     	while (!WindowShouldClose()) {
        	HideCursor();

         	float dt   = GetFrameTime();
         	Vector2 mouse = GetMousePosition();

         // Initialize on first frame
         	if (firstFrame) {
             		prevMouse  = mouse;
             		firstFrame = false;

			for (int i = 0; i < MAX_ASTEROIDS; i++) {
				init_asteroid(&asteroids[i], (Vector2){ rand() % 800, rand() % 450 });
			}

         	}

         // Direction = mouse velocity
         	Vector2 delta = Vector2Subtract(mouse, prevMouse);
         	float speed2  = delta.x*delta.x + delta.y*delta.y;

         // Update facing only if mouse moved a bit
         	if (speed2 > 0.1f) {
             		angle = atan2f(delta.y, delta.x);
         	}

         // Apex/pivot is the mouse
         	Vector2 apex = mouse;

         	Vector2 baseL = (Vector2){ -halfBase,  height };
         	Vector2 baseR = (Vector2){  halfBase,  height };

         // Rotate so the triangle faces movement direction
         	Vector2 rotL = Vector2Rotate(baseL, angle + PI/2.0f);
         	Vector2 rotR = Vector2Rotate(baseR, angle + PI/2.0f);

         // Final vertices
         	triangleV1 = apex;                 // top corner at mouse
         	triangleV2 = Vector2Add(apex, rotL);
         	triangleV3 = Vector2Add(apex, rotR);
	
         // spawn bullet
         if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
             // direction from angle
             	Vector2 dir = (Vector2){ cosf(angle), sinf(angle) };
             	if (Vector2LengthSqr(dir) < 1e-6f) dir = (Vector2){0, -1};

             	for (int i = 0; i < MAX_BULLETS; i++) {
                 	if (!bullets[i].alive) {
                     		bullets[i].pos   = apex;               // start at ship tip
                     		bullets[i].vel   = Vector2Scale(dir, 500.0f); // 500 px/s
                     		bullets[i].r     = 5.0f;
                     		bullets[i].alive = true;
                     	break;
                 	}
             	}
         }

         // --- Update bullets ---
         for (int i = 0; i < MAX_BULLETS; i++) {
             	if (!bullets[i].alive) continue;

             		bullets[i].pos = Vector2Add(
                 	bullets[i].pos,
                 	Vector2Scale(bullets[i].vel, dt)
             		);

             // kill bullets off-screen
        	if (bullets[i].pos.x < -20 || bullets[i].pos.x > screenWidth + 20 ||
                	bullets[i].pos.y < -20 || bullets[i].pos.y > screenHeight + 20) {
                 	bullets[i].alive = false;
             		}
         	}

         // Prepare for next frame
         prevMouse = mouse;

        BeginDrawing();
        	ClearBackground(BLACK);

             	DrawTriangle(triangleV1, triangleV2, triangleV3, WHITE);

             	for (int i = 0; i < MAX_BULLETS; i++) {
        		if (bullets[i].alive) {
                     		DrawCircleV(bullets[i].pos, bullets[i].r, SKYBLUE);
                 	}
             	}
		for (int i = 0; i < MAX_ASTEROIDS; i++) {
    			update_asteroid(&asteroids[i], dt);
    			draw_asteroid(&asteroids[i]);
		}

        EndDrawing();
	}

	return 0;
}
