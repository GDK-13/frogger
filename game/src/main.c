#include "raylib.h"
#include "player.h"
#include "enemies.h"
#include "hud.h"
#include "animation.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Caminhos dos sprites dos carros
const char *car_sprites[] = {
    "resources/sprites/frogbreath_sp/caminhao.png",
    "resources/sprites/frogbreath_sp/carro1.png",
    "resources/sprites/frogbreath_sp/carro2.png",
    "resources/sprites/frogbreath_sp/carro3.png",
    "resources/sprites/frogbreath_sp/carro4.png"
};

// Estrutura global para armazenar os dados do jogador
struct player player;

// Lista dinâmica para carros ativos
EnemyCar *active_cars = NULL;
int active_car_count = 0;

// Temporizadores de spawn para cada faixa
float spawn_timers[5] = {0};

// Limite máximo de carros na tela
const int max_cars_on_screen = 10;

// Posições Y das faixas
const int lane_y_positions[] = {289, 321, 353, 385, 415};
#define NUM_LANES (sizeof(lane_y_positions) / sizeof(lane_y_positions[0]))

int main() {
    // --- Inicialização da janela ---
    const int screen_width = 448;
    const int screen_height = 512;
    InitWindow(screen_width, screen_height, "Frogger 2025");
    SetTargetFPS(60);

    // --- Inicialização do áudio ---
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        printf("Erro: Não foi possível inicializar o dispositivo de áudio\n");
        CloseWindow();
        return 1;
    }

    // --- Inicialização de recursos ---
    srand(time(NULL)); // Semente para números aleatórios

    // --- Texturas ---
    Texture2D background = LoadTexture("resources/bg/frogbreath_bg.png");
    if (background.id == 0) {
        printf("Erro: Não foi possível carregar a textura 'resources/bg/frogbreath_bg.png'\n");
        CloseWindow();
        return 1;
    }
    Texture2D life = LoadTexture("resources/sprites/life.png");
    Texture2D frog = LoadTexture("resources/sprites/frog.png");

    // --- Sons ---
    Sound effects[4] = {
        LoadSound("resources/sounds/completion.wav"), // Efeito de completar fase
        LoadSound("resources/sounds/death.wav"),      // Efeito de morte
        LoadSound("resources/sounds/jump.wav"),       // Efeito de pulo
        LoadSound("resources/sounds/powerUp.wav")     // Efeito de power-up
    };
    SetSoundVolume(effects[0], 0.50f);
    SetSoundVolume(effects[1], 0.80f);
    SetSoundVolume(effects[2], 0.25f);
    SetSoundVolume(effects[3], 0.50f);

    // --- Música ---
    Music frogsoath = LoadMusicStream("resources/sounds/frogsoath.ogg");

    // --- Fontes ---
    Font font = LoadFontEx("resources/fontes/8_bit_fortress/8-bit-fortress.ttf", 32, NULL, 0);
    if (font.baseSize == 0) {
        printf("Erro: Não foi possível carregar a fonte 8-bit\n");
        CloseWindow();
        return 1;
    }

    // --- Inicialização do jogador ---
    player_init(&player, "resources/sprites/sapo-ani.png", (Vector2){224, 448});

    // --- Inicia a música de fundo ---
    PlayMusicStream(frogsoath);

    // --- Loop principal do jogo ---
    while (!WindowShouldClose()) {
        UpdateMusicStream(frogsoath); // Atualiza a música de fundo
        float dt = GetFrameTime();    // Tempo decorrido desde o último frame

        // --- Spawn de carros por faixa ---
        for (int i = 0; i < NUM_LANES; i++) {
            spawn_timers[i] -= dt;
            if (spawn_timers[i] <= 0) {
                spawn_timers[i] = rand() % 3 + 1; // Tempo aleatório para próximo spawn
                spawn_car(&active_cars, &active_car_count, car_sprites[i],
                          (i % 2 == 0) ? 100.0f : -120.0f, i, max_cars_on_screen, lane_y_positions);
            }
        }

        // --- Atualização dos carros ativos ---
        for (int i = 0; i < active_car_count; i++) {
            active_cars[i].update(&active_cars[i], dt);

            // Remove carros que saíram da tela
            if ((active_cars[i].speed > 0 && active_cars[i].position.x > GetScreenWidth()) ||
                (active_cars[i].speed < 0 && active_cars[i].position.x < -active_cars[i].texture.width)) {
                active_cars[i].unload(&active_cars[i]);
                for (int j = i; j < active_car_count - 1; j++) {
                    active_cars[j] = active_cars[j + 1];
                }
                active_car_count--;
                active_cars = realloc(active_cars, active_car_count * sizeof(EnemyCar));
                i--;
            }
        }

        // --- Atualização do jogador ---
        player_update(&player, dt, 32, 32, &effects[2]);

        // --- Verificação de colisões entre jogador e carros ---
        if (!player.is_dead && !player.game_over) {
            for (int i = 0; i < active_car_count; i++) {
                if (active_cars[i].check_collision(&active_cars[i], player.hitbox)) {
                    player_die(&player, &effects[1]);
                    break;
                }
            }
        }

        // --- Desenho de todos os elementos na tela ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Fundo
        DrawTexture(background, 0, 0, WHITE);

        // Jogador (vivo ou morto)
        if (!player.is_dead) {
            draw_player(&player, 32, 32, 5);
        } else {
            dead_player(&player, 32, 32, 3);
        }

        // Carros inimigos
        for (int i = 0; i < active_car_count; i++) {
            active_cars[i].draw(&active_cars[i]);
        }

        // HUD (vidas, score, etc)
        draw_hud(font, player.lives, player.score, life);

        // --- Tela de Game Over ---
        if (player.game_over) {
            DrawText("GAME OVER", 140, 200, 40, RED);
            DrawText("Pressione ESPAÇO para recomeçar", 80, 260, 20, BLACK);
            if (IsKeyPressed(KEY_SPACE)) {
                // Reinicia o estado do jogador
                player.game_over = false;
                player.lives = 5;
                player.score = 0;
                player.position = player.start_position;
                player.target_position = player.start_position;
                player.anim.current_frame = 0;
                player.rotation = 0.0f;
                player.is_dead = false;
                player.min_y_position = player.start_position.y;
                for (int i = 0; i < 5; i++) {
                    player.oc_houses[i] = false;
                }
            }
        }

        // --- Verifica se o jogador chegou ao destino ---
        get_home(&player, &frog); // Verifica se o jogador chegou ao destino

        EndDrawing();
    }

    // --- Liberação de recursos ---
    // Libera carros ativos
    for (int i = 0; i < active_car_count; i++) {
        active_cars[i].unload(&active_cars[i]);
    }
    free(active_cars);

    // Libera texturas
    UnloadTexture(background);
    UnloadTexture(life);
    unloadTexture(frog);

    // Libera sons
    for (int i = 0; i < 4; i++) {
        UnloadSound(effects[i]);
    }
    StopMusicStream(frogsoath);

    // Libera música
    UnloadMusicStream(frogsoath);

    // Libera recursos do jogador
    player_unload(&player);

    // Libera fontes
    UnloadFont(font);

    // Fecha dispositivos e janela
    CloseAudioDevice();
    CloseWindow();

    return 0;
}