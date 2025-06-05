#include "raylib.h"
#include "player.h"
#include "enemies.h"
#include <stdio.h>



int main() {
    const int screen_width = 448;
    const int screen_height = 512;

    InitWindow(screen_width, screen_height, "raylib example");
    SetTargetFPS(60);

    Texture2D background = LoadTexture("resources/bg/frogbreath_bg.png");
    Texture2D player = LoadTexture("resources/sprites/frogbreath_sp/sidle.png");
    Enemy enemies[5] = {
        {{448, 416}, 2, LoadTexture("resources/sprites/frogbreath_sp/carro1.png")},
        {{0, 384}, 2, LoadTexture("resources/sprites/frogbreath_sp/carro2.png")},
        {{448, 352}, 1, LoadTexture("resources/sprites/frogbreath_sp/carro3.png")},
        {{0, 320}, 3, LoadTexture("resources/sprites/frogbreath_sp/carro4.png")},
        {{0, 288}, 2, LoadTexture("resources/sprites/frogbreath_sp/caminhao.png")}
    };


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
        
        UpdateEnemies(enemies, false, 0);
        UpdateEnemies(enemies, true, 1);
        UpdateEnemies(enemies, false, 2);
        UpdateEnemies(enemies, true, 3);
        UpdateEnemies(enemies, true, 4);

        DrawEnemies(enemies);
        EndDrawing();
    }

    for (int i = 0; i < 5; i++)
    {
        UnloadTexture(enemies[i].texture);
    }

    UnloadTexture(player);
    UnloadTexture(background);

    CloseWindow();
    return 0;
}
