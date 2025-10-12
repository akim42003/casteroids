#include <stdio.h>
#include <stdlib.h>
#include "lib/raylib.h"
#include "ship.h"

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Casteroids!");

    // const int initX = rand() % (201)+200;
    // const int initY = rand() % (201)+200;

    ship();

    CloseWindow();

    return 0;
}
