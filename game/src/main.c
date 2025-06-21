<<<<<<< HEAD
#include "raylib.h"
#include "player.h"
#include "enemies.h"
#include "hud.h"
#include "animation.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
=======
#include "raylib.h"       
#include "player.h"       
#include "enemies.h"  
#include "trunk.h"    
#include "animation.h"    
#include <stdio.h>        
#include <math.h>         
#include <stdlib.h>       
#include <time.h>         
>>>>>>> 8651578 (Troncos)

// Caminhos dos sprites dos carros
const char *car_sprites[] = {
    "resources/sprites/frogbreath_sp/caminhao.png",
    "resources/sprites/frogbreath_sp/carro1.png",
    "resources/sprites/frogbreath_sp/carro2.png",
    "resources/sprites/frogbreath_sp/carro3.png",
    "resources/sprites/frogbreath_sp/carro4.png"
};

const char *trunk_sprites[] = {
    "resources/sprites/frogbreath_sp/tronco1.png",
    "resources/sprites/frogbreath_sp/tronco2.png",
    "resources/sprites/frogbreath_sp/tronco3.png",
    "resources/sprites/frogbreath_sp/tronco4.png"
};

const char *two_part_trunk[] = {
    "resources/sprites/frogbreath_sp/tronco1.png",
    "resources/sprites/frogbreath_sp/tronco4.png"
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

// Lista dinâmica para carros ativos
Trunk *trunk = NULL; // Array dinâmico para armazenar os troncos ativos
int trunk_count = 0;    // Contador de troncos ativos

// Número total de sprites
int num_sprites = sizeof(trunk_sprites) / sizeof(trunk_sprites[0]);

// Temporizadores de spawn para cada faixa
float trunk_spawn_timers[4] = {0}; // Temporizadores para controlar o spawn de troncos em cada faixa

// Limite máximo de troncos na tela
const int max_trunk_on_screen = 10;
// Faixas horizontais que cada tronco pode passar
const int lanetrunk_y_positions[] = {225, 186, 140, 97}; // Posições Y das faixas
#define NUM_LANE_TRUNK (sizeof(lanetrunk_y_positions) / sizeof(lanetrunk_y_positions[0]))

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

<<<<<<< HEAD
        // --- Atualização dos carros ativos ---
=======
        //Atualiza os temporizadores de spawn de troncos
        for (int i = 0; i < NUM_LANE_TRUNK; i++) {
        trunk_spawn_timers[i] -= dt;
        if (trunk_spawn_timers[i] <= 0) {
            trunk_spawn_timers[i] = rand() % 3 + 1;
            float speed = (i % 2 == 0) ? 50.0f : -70.0f;
            int parts = (rand() % 2 == 0) ? 2 : 4; // Tronco pequeno ou grande
            const char **sprite_set = (parts == 2) ? two_part_trunk : trunk_sprites;
            bool can_spawn = true;
            for (int j = 0; j < trunk_count; j++) {
                if (trunk[j].lane == i) {
                    float distance = fabsf(trunk[j].position.x - 0); // Posição inicial depende da direção
                     if (distance < 50.0f) { // valor arbitrário, ajuste conforme o tamanho do tronco
                    can_spawn = false;
                     break;
                }
            }
        }
           if(can_spawn) {
            spawn_trunk(&trunk, &trunk_count, sprite_set, speed, i, max_trunk_on_screen, lanetrunk_y_positions, parts);
        }
    }
}
        // Atualiza os carros ativos
>>>>>>> 8651578 (Troncos)
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

<<<<<<< HEAD
        // --- Atualização do jogador ---
        player_update(&player, dt, 32, 32, &effects[2]);

        // --- Verificação de colisões entre jogador e carros ---
        if (!player.is_dead && !player.game_over) {
=======
        // Atualiza os troncos ativos
        for (int i = 0; i < trunk_count; i++) {
            trunk[i].update(&trunk[i], dt);
            if ((trunk[i].speed > 0 && trunk[i].position.x > GetScreenWidth()) ||
                (trunk[i].speed < 0 && trunk[i].position.x + 128 < 0)) { 
                trunk[i].unload(&trunk[i]); // Libera os recursos do carro
                for (int j = i; j < trunk_count - 1; j++) {
                    trunk[j] = trunk[j + 1]; // Reorganiza o array
                }
                trunk_count--; // Reduz o contador de carros ativos
                trunk = realloc(trunk, trunk_count * sizeof(Trunk));
                i--; // Ajusta o índice
        }
    }
    
        bool on_trunk = player_on_trunk (&player, trunk, trunk_count, dt);

        // Atualiza o jogador
        player_update(&player, dt, 32, 32); // Atualiza a posição e estado do jogador

        if (player.position.x < 0 || player.position.x > GetScreenWidth() - 32) {
            player_die(&player); // Mata o jogador se ele sair da tela
        }
        
        if (player.position.y >= 230 && player.position.y < 80 && !on_trunk) {
            player_die(&player); // Afoga o jogador
        }

        // Verifica colisões
        if (!player.is_dead && !player.game_over) { // Apenas verifica se o jogador está vivo e o jogo não acabou
>>>>>>> 8651578 (Troncos)
            for (int i = 0; i < active_car_count; i++) {
                if (active_cars[i].check_collision(&active_cars[i], player.hitbox)) {
                    player_die(&player, &effects[1]);
                    break;
                }
            }
        }
        
    // Definindo a área da água 
    Rectangle water_area = {0, 100, GetScreenWidth(), 158};

<<<<<<< HEAD
        // --- Desenho de todos os elementos na tela ---
        BeginDrawing();
        ClearBackground(RAYWHITE);
=======
    if (CheckCollisionPointRec(player.position, water_area)) {
    bool on_trunck = player_on_trunk(&player, trunk, trunk_count, dt);
    
    if (!on_trunk) {
        player_die(&player); // afoga o jogador
    }
}
        BeginDrawing(); // Inicia o desenho na tela
        ClearBackground(RAYWHITE); // Limpa a tela com cor branca
>>>>>>> 8651578 (Troncos)

        // Fundo
        DrawTexture(background, 0, 0, WHITE);

<<<<<<< HEAD
        // Jogador (vivo ou morto)
        if (!player.is_dead) {
            draw_player(&player, 32, 32, 5);
=======
        DrawRectangleRec(water_area, (Color){0, 0, 255, 100});

        // Desenha os troncos
        for (int i = 0; i < trunk_count; i++) {
            trunk[i].draw(&trunk[i]);
        }

        // Desenha o jogador (vivo ou morto)
        if (!player.is_dead) {
            player_update(&player, dt, 32, 32);
            draw_player(&player, 32, 32, 5); // Desenha o jogador vivo
>>>>>>> 8651578 (Troncos)
        } else {
            dead_player(&player, 32, 32, 3);
        }

        // Carros inimigos
        for (int i = 0; i < active_car_count; i++) {
            active_cars[i].draw(&active_cars[i]);
        }

<<<<<<< HEAD
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
=======
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

                active_car_count = 0;
                free(active_cars);
                 active_cars = NULL;

                trunk_count = 0;
                free(trunk);
                trunk = NULL;

                // Resetar timers de spawn
                for (int i = 0; i < NUM_LANES; i++) spawn_timers[i] = 0;
                for (int i = 0; i < NUM_LANE_TRUNK; i++) trunk_spawn_timers[i] = 0;
>>>>>>> 8651578 (Troncos)
            }
        }

        // --- Verifica se o jogador chegou ao destino ---
        get_home(&player, &frog); // Verifica se o jogador chegou ao destino

        EndDrawing();
    }

<<<<<<< HEAD
    // --- Liberação de recursos ---
    // Libera carros ativos
=======
    // Libera os recursos
    for (int i = 0; i < trunk_count; i++) {
        trunk[i].unload(&trunk[i]);  // Libera os recursos de cada tronco ativo
    }
    // Libera os recursos
>>>>>>> 8651578 (Troncos)
    for (int i = 0; i < active_car_count; i++) {
        active_cars[i].unload(&active_cars[i]);
    }
<<<<<<< HEAD
    free(active_cars);
=======
    free(active_cars); // Libera o array dinâmico
    free(trunk);
    UnloadTexture(background); // Libera a textura de fundo
    player_unload(&player); // Libera os recursos do jogador
    CloseWindow(); // Fecha a janela
>>>>>>> 8651578 (Troncos)

    // Libera texturas
    UnloadTexture(background);
    UnloadTexture(life);
    UnloadTexture(frog);

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