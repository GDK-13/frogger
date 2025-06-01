#include "raylib.h"
 
int main()
{
    InitWindow(800, 600, "raylib example");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
        ClearBackground(RAYWHITE);

        DrawText("hello raylib", 10, 10, 32, BLACK);

    
        }
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
