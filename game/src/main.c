#include "raylib.h"
#include "player.h"
#include "animation.h"
#include <stdio.h>
#include <stdlib.h>


struct player player;

int main() {
    const int screen_width = 448;
    const int screen_height = 512;

    InitWindow(screen_width, screen_height, "raylib example");
    SetTargetFPS(60);

    Texture2D background = LoadTexture("resources/bg/frogbreath_bg.png");
    if (background.id == 0) {
        printf("Erro: Não foi possível carregar a textura 'resources/bg/frogbreath_bg.png'\n");
        CloseWindow();
        return 1;
    }

    player_init(&player, "resources/sprites/sapo-ani.png", (Vector2){ 256, 448 });

    Animation player_animation = {
        .first_frame = 0,
        .last_frame = 4,    
        .current_frame = 0,
        .duration_left = 0.1f,
        .speed = 0.1f 
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        animation_update(&player_animation, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);

        // Atualiza e desenha o jogador
        player_update(&player, dt, 32, 32);
        draw_player(&player, 32, 32, 5);

        // Desenha a hitbox do jogador para teste
        DrawRectangleLines(
            player.hitbox.x,
            player.hitbox.y,
            player.hitbox.width,
            player.hitbox.height,
            RED
        );
        DrawRectangleLines(
            0,
            96,
            background.width,
            background.height-96,
            RED
        );

        EndDrawing();
    
    }

    UnloadTexture(background);
    player_unload(&player);
    CloseWindow();

    return 0;
}
