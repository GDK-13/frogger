#include "raylib.h"
#include "player.h"
#include "enemies.h"
#include "animation.h"
#include <stdio.h>
#include <stdlib.h>

// Definir o total de carros na tela
#define NUM_CARS 5

// Caminho dos sprites que serão carregados
const char *car_sprites[NUM_CARS] = {
    "resources/sprites/frogbreath_sp/caminhao.png",
    "resources/sprites/frogbreath_sp/carro1.png",
    "resources/sprites/frogbreath_sp/carro2.png",
    "resources/sprites/frogbreath_sp/carro3.png",
    "resources/sprites/frogbreath_sp/carro4.png",
};

// Estrutura global para armazenar os dados do jogador
struct player player;
// Estrutura global para armazenar os dados dos carros
struct enemyCar enemyCar[NUM_CARS];

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
    player_init(&player, "resources/sprites/sapo-ani.png", (Vector2){ 208, 448});

    // Faixas horizontais que cada carro vai passar
    const int lane_y_positions[NUM_CARS] = {289, 321, 353, 385, 415};

    for (int i = 0; i < NUM_CARS; i++) {
        float y = lane_y_positions[i];
        float speed = (i % 2 == 0) ? 100.0f : -120.0f;
        float x = (speed > 0) ? -100 : screen_width + 100;

        enemy_init(&enemyCar[i], car_sprites[i], (Vector2) {x, y}, speed);
    }

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        float dt = GetFrameTime(); // Obtém o tempo decorrido entre frames

        // Atualiza a posição dos carros
        for (int i = 0; i < NUM_CARS; i++) {
            enemy_update(&enemyCar[i], dt);
        }
        // Atualiza o movimento, animação, posição alvo e hitbox do jogador 
        player_update(&player, dt, 32, 32);

        // Verifica as colisões
        if (!player.is_dead && !player.game_over) {
            for (int i = 0; i < NUM_CARS;i++) {
                if (enemy_check_collision(&enemyCar[i], player.hitbox)) {
                    player_die(&player);
                    break;
                }
            }
        }
        BeginDrawing();
        ClearBackground(RAYWHITE); // Limpa a tela com cor branca

        // Desenha o fundo
        DrawTexture(background, 0, 0, WHITE);

        // Verifica se o jogador está vivo ou morto e desenha a animação necessária
        if (!player.is_dead) {
            draw_player(&player, 32, 32, 5);
        } else {
            dead_player(&player,32, 32, 3);
        }

        // Desenha os carros
        for (int i = 0; i < NUM_CARS; i++) {
            enemy_draw(&enemyCar[i]);
        }

        DrawText(TextFormat("Vidas: %d", player.lives), 10, 10, 20, BLACK);
        DrawText(TextFormat("Score: %d",player.score), 320, 10, 20, BLACK);

        if(player.game_over) {
            DrawText("GAME OVER", 140, 200, 40, RED);
            DrawText("Pressione ESPAÇO para recomeçar", 80, 260, 20, BLACK);
            // Renicializa todos os parâmetros do jogador
            if (IsKeyPressed(KEY_SPACE)) {
                player.game_over = false;
                player.lives = 3;
                player.score = 0;
                player.position = player.start_position;
                player.target_position = player.start_position;
                player.anim.current_frame = 0;
                player.rotation = 0.0f;
                player.is_dead = false;
                player.min_y_position = player.start_position.y;
            }
        }

        // Desenha a hitbox do jogador para teste
        DrawRectangleLines(
            player.hitbox.x,
            player.hitbox.y,
            player.hitbox.width,
            player.hitbox.height,
            RED
        );
        // Hitbox teste
        for (int i = 0; i < NUM_CARS; i++) {
        DrawRectangleLines(
            enemyCar[i].hitbox.x,
            enemyCar[i].hitbox.y,
            enemyCar[i].hitbox.width,
            enemyCar[i].hitbox.height,
            RED
        );
        }

        EndDrawing();
    }

    // Libera os recursos
    for (int i = 0; i < NUM_CARS; i++) {
        enemy_unload(&enemyCar[i]);
    }
    UnloadTexture(background);
    player_unload(&player);
    CloseWindow();

    return 0;
}
