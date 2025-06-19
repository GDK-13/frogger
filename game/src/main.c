#include "raylib.h"       
#include "player.h"       
#include "enemies.h"      
#include "animation.h"    
#include <stdio.h>        
#include <math.h>         
#include <stdlib.h>       
#include <time.h>         

// Caminho dos sprites que serão carregados
const char *car_sprites[] = {
    "resources/sprites/frogbreath_sp/caminhao.png", // Caminhão
    "resources/sprites/frogbreath_sp/carro1.png",  // Carro 1
    "resources/sprites/frogbreath_sp/carro2.png",  // Carro 2
    "resources/sprites/frogbreath_sp/carro3.png",  // Carro 3
    "resources/sprites/frogbreath_sp/carro4.png"   // Carro 4
};

// Estrutura global para armazenar os dados do jogador
struct player player;

// Lista dinâmica para carros ativos
EnemyCar *active_cars = NULL; // Array dinâmico para armazenar os carros ativos
int active_car_count = 0;    // Contador de carros ativos

// Temporizadores de spawn para cada faixa
float spawn_timers[5] = {0}; // Temporizadores para controlar o spawn de carros em cada faixa

// Limite máximo de carros na tela
const int max_cars_on_screen = 10;

// Faixas horizontais que cada carro pode passar
const int lane_y_positions[] = {289, 321, 353, 385, 415}; // Posições Y das faixas
#define NUM_LANES (sizeof(lane_y_positions) / sizeof(lane_y_positions[0])) // Número de faixas

int main() {
    const int screen_width = 448; // Largura da janela
    const int screen_height = 512; // Altura da janela

    // Inicializa a janela do jogo
    InitWindow(screen_width, screen_height, "raylib example"); // Cria uma janela com título "raylib example"
    SetTargetFPS(60); // Define o FPS alvo para 60

    // Inicializa o gerador de números aleatórios
    srand(time(NULL)); // Semente para geração de números aleatórios baseada no tempo atual

    // Carrega a textura de fundo
    Texture2D background = LoadTexture("resources/bg/frogbreath_bg.png"); // Carrega a textura do fundo
    if (background.id == 0) { // Verifica se a textura foi carregada corretamente
        printf("Erro: Não foi possível carregar a textura 'resources/bg/frogbreath_bg.png'\n");
        CloseWindow(); // Fecha a janela
        return 1; // Sai do programa com erro
    }

    // Inicializa o jogador
    player_init(&player, "resources/sprites/sapo-ani.png", (Vector2){208, 448}); // Inicializa o jogador com posição inicial e sprite

    // Loop principal do jogo
    while (!WindowShouldClose()) { // Continua enquanto a janela não for fechada
        float dt = GetFrameTime(); // Obtém o tempo decorrido entre frames

        // Atualiza os temporizadores de spawn
        for (int i = 0; i < NUM_LANES; i++) {
            spawn_timers[i] -= dt; // Reduz o temporizador
            if (spawn_timers[i] <= 0) { // Se o temporizador atingir 0
                spawn_timers[i] = rand() % 3 + 1; // Define um tempo aleatório para o próximo spawn
                spawn_car(&active_cars, &active_car_count, car_sprites[i], (i % 2 == 0) ? 100.0f : -120.0f, i, max_cars_on_screen, lane_y_positions); // Spawn do carro
            }
        }

        // Atualiza os carros ativos
        for (int i = 0; i < active_car_count; i++) {
            active_cars[i].update(&active_cars[i], dt); // Atualiza a posição do carro

            // Remove carros que saíram da tela
            if ((active_cars[i].speed > 0 && active_cars[i].position.x > GetScreenWidth()) || // Carro indo para a direita
                (active_cars[i].speed < 0 && active_cars[i].position.x < -active_cars[i].texture.width)) { // Carro indo para a esquerda
                active_cars[i].unload(&active_cars[i]); // Libera os recursos do carro
                for (int j = i; j < active_car_count - 1; j++) {
                    active_cars[j] = active_cars[j + 1]; // Reorganiza o array
                }
                active_car_count--; // Reduz o contador de carros ativos
                active_cars = realloc(active_cars, active_car_count * sizeof(EnemyCar)); // Realoca memória para o array
                i--; // Ajusta o índice para evitar pular o próximo carro
            }
        }

        // Atualiza o jogador
        player_update(&player, dt, 32, 32); // Atualiza a posição e estado do jogador

        // Verifica colisões
        if (!player.is_dead && !player.game_over) { // Apenas verifica se o jogador está vivo e o jogo não acabou
            for (int i = 0; i < active_car_count; i++) {
                if (active_cars[i].check_collision(&active_cars[i], player.hitbox)) { // Verifica colisão entre jogador e carro
                    player_die(&player); // Mata o jogador
                    break; // Sai do loop
                }
            }
        }

        BeginDrawing(); // Inicia o desenho na tela
        ClearBackground(RAYWHITE); // Limpa a tela com cor branca

        // Desenha o fundo
        DrawTexture(background, 0, 0, WHITE); // Desenha a textura de fundo

        // Desenha o jogador (vivo ou morto)
        if (!player.is_dead) {
            draw_player(&player, 32, 32, 5); // Desenha o jogador vivo
        } else {
            dead_player(&player, 32, 32, 3); // Desenha o jogador morto
        }

        // Desenha os carros ativos
        for (int i = 0; i < active_car_count; i++) {
            active_cars[i].draw(&active_cars[i]); // Desenha cada carro ativo
        }

        // Exibe informações na tela
        DrawText(TextFormat("Vidas: %d", player.lives), 10, 10, 20, BLACK); // Exibe o número de vidas
        DrawText(TextFormat("Score: %d", player.score), 320, 10, 20, BLACK); // Exibe o score

        if (player.game_over) { // Se o jogo acabou
            DrawText("GAME OVER", 140, 200, 40, RED); // Exibe "GAME OVER"
            DrawText("Pressione ESPAÇO para recomeçar", 80, 260, 20, BLACK); // Instrução para reiniciar
            if (IsKeyPressed(KEY_SPACE)) { // Se a tecla espaço for pressionada
                player.game_over = false; // Reseta o estado do jogo
                player.lives = 3; // Reseta as vidas
                player.score = 0; // Reseta o score
                player.position = player.start_position; // Reseta a posição do jogador
                player.target_position = player.start_position; // Reseta a posição alvo
                player.anim.current_frame = 0; // Reseta a animação
                player.rotation = 0.0f; // Reseta a rotação
                player.is_dead = false; // Marca o jogador como vivo
                player.min_y_position = player.start_position.y; // Reseta a posição mínima
            }
        }

        EndDrawing(); // Finaliza o desenho na tela
    }

    // Libera os recursos
    for (int i = 0; i < active_car_count; i++) {
        active_cars[i].unload(&active_cars[i]); // Libera os recursos de cada carro ativo
    }
    free(active_cars); // Libera o array dinâmico
    UnloadTexture(background); // Libera a textura de fundo
    player_unload(&player); // Libera os recursos do jogador
    CloseWindow(); // Fecha a janela

    return 0; // Retorna 0 indicando execução bem-sucedida
}