#include "raylib.h"
#include "player.h"
#include <stdio.h>

int main() {
    const int screen_width = 448;
    const int screen_height = 512;

    InitWindow(screen_width, screen_height, "raylib example");
    SetTargetFPS(60);

    Texture2D background = LoadTexture("resources/bg/frogbreath_bg.png");
    Texture2D player = LoadTexture("resources/sprites/frogbreath_sp/sidle.png");

    if (background.id == 0) {
        printf("Erro ao carregar o background!\n");
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose()) {
        UpdatePlayer();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);
        DrawPlayer(player);
        EndDrawing();
    }

    UnloadTexture(player);
    UnloadTexture(background);

    CloseWindow();
    return 0;
}
