#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

Texture2D background;

int main()
{
    const int screen_width = 448;
    const int screen_height = 512;



    InitWindow(screen_width, screen_height, "raylib example");
    SetTargetFPS(60);

    background = LoadTexture("resources/bg/frogbreath_bg.png");


    if (background.id == 0) {
        printf("Erro ao carregar o background!\n");
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTexturePro(
                background,
                (Rectangle){0, 0, screen_width, screen_height},  // Origem da textura (src)
                (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, // Destino da textura (dst)
                (Vector2){0, 0},
                0.0f,
                WHITE
            );
        }
        EndDrawing();
    }

    UnloadTexture(background);
    CloseWindow();

    return 0;
}
