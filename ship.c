#include <stdio.h>
#include <stdlib.h>
#include "lib/raylib.h"
#include "lib/raymath.h"

int ship(){
    const int screenWidth = 800;

    Vector2 triangleV1 = { screenWidth/2.0f + 15.0f, 370.0f };
    Vector2 triangleV2 = { screenWidth/2.0f , 400.0f };
    Vector2 triangleV3 = { screenWidth/2.0f + 30.0f, 400.0f };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        HideCursor();
        ClearBackground(BLACK);

        // constants
        const float halfBase = 12.0f;
        const float height   = 30.0f;

        // persistent state
        static Vector2 prevMouse = {0};
        static bool    firstFrame = true;
        static float   angle = -PI/2.0f; // default: pointing "up"

        Vector2 mouse = GetMousePosition();

        // Initialize on first frame
        if (firstFrame || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            prevMouse = mouse;
            firstFrame = false;
        }

        // Direction = mouse velocity
        Vector2 delta = Vector2Subtract(mouse, prevMouse);
        float speed2 = delta.x*delta.x + delta.y*delta.y;

        // Update facing only if mouse moved a bit
        if (speed2 > 0.1f) {
            angle = atan2f(delta.y, delta.x);
        }

        // Apex/pivot is the mouse
        Vector2 apex = mouse;

        // Local base corners (triangle pointing up by default)
        Vector2 baseL = (Vector2){ -halfBase,  height };
        Vector2 baseR = (Vector2){  halfBase,  height };

        // Rotate so the triangle faces movement direction
        Vector2 rotL = Vector2Rotate(baseL, angle + PI/2.0f);
        Vector2 rotR = Vector2Rotate(baseR, angle + PI/2.0f);

        // Final vertices
        triangleV1 = apex;                 // top corner at mouse
        triangleV2 = Vector2Add(apex, rotL);
        triangleV3 = Vector2Add(apex, rotR);

        // Prepare for next frame
        prevMouse = mouse;

        BeginDrawing();
            DrawTriangle(triangleV1, triangleV2, triangleV3, WHITE);
        EndDrawing();
    }

    return 0;
}
