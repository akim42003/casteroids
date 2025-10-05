#include <stdio.h>
#include <stdlib.h>
#include "lib/raylib.h"
#include "lib/raymath.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Casteroids!");

    // const int initX = rand() % (201)+200;
    // const int initY = rand() % (201)+200;


    Vector2 triangleV1 = { screenWidth/2.0f + 15.0f, 370.0f };
    Vector2 triangleV2 = { screenWidth/2.0f , 400.0f };
    Vector2 triangleV3 = { screenWidth/2.0f + 30.0f, 400.0f };
    Vector2 triangle_pos = triangleV1;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        ClearBackground(BLACK);
        triangle_pos = GetMousePosition();

        triangleV1.x = triangle_pos.x + 15;
        triangleV1.y = triangle_pos.y - 30;
        triangleV2 = triangle_pos;
        triangleV3.x = triangle_pos.x + 30;
        triangleV3.y = triangle_pos.y;

        BeginDrawing();
            DrawTriangle(triangleV1, triangleV2, triangleV3, WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
