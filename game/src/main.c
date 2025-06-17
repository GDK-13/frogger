#include "raylib.h"
#include "player.h"
#include "animation.h"
#include <stdio.h>
#include <stdlib.h>


// Estrutura global para armazenar os dados do jogador
struct player player;

int main() {
    const int screen_width = 448; // Largura da janela
    const int screen_height = 512; // Altura da janela

    // Inicializa a janela do jogo
    InitWindow(screen_width, screen_height, "raylib example");
    SetTargetFPS(60); // Define o FPS alvo

    // Carrega a textura de fundo
    Texture2D background = LoadTexture("resources/bg/frogbreath_bg.png");
    if (background.id == 0) {
        printf("Erro: Não foi possível carregar a textura 'resources/bg/frogbreath_bg.png'\n");
        CloseWindow();
        return 1;
    }

    // Inicializa o jogador
    player_init(&player, "resources/sprites/sapo-ani.png", (Vector2){ 256, 448 });

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        float dt = GetFrameTime(); // Obtém o tempo decorrido entre frames

        BeginDrawing();
        ClearBackground(RAYWHITE); // Limpa a tela com cor branca

        // Desenha o fundo
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

        EndDrawing();
    }

    // Libera os recursos
    UnloadTexture(background);
    player_unload(&player);
    CloseWindow();

    return 0;
}
