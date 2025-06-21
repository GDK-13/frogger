#include "raylib.h" // Biblioteca principal da Raylib para gráficos, áudio, etc.
#include "player.h" // Header do jogador
#include "enemies.h" // Header dos inimigos (carros)
#include "trunk.h" // Header dos troncos
#include "events.h" // Header dos eventos do jogo
#include "animation.h" // Header das animações
#include "hud.h" // Header do HUD (placar, vidas, etc)
#include <stdio.h> // Biblioteca padrão de entrada/saída
#include <math.h> // Biblioteca matemática
#include <stdlib.h> // Biblioteca padrão (malloc, free, rand, etc)
#include <time.h> // Biblioteca para manipulação de tempo

// Caminhos dos sprites dos carros
const char *car_sprites[] = {
    "resources/sprites/frogbreath_sp/caminhao.png",
    "resources/sprites/frogbreath_sp/carro1.png",
    "resources/sprites/frogbreath_sp/carro2.png",
    "resources/sprites/frogbreath_sp/carro3.png",
    "resources/sprites/frogbreath_sp/carro4.png"
};

// Caminhos dos sprites dos troncos
const char *trunk_sprites[] = {
    "resources/sprites/frogbreath_sp/tronco1.png",
    "resources/sprites/frogbreath_sp/tronco2.png",
    "resources/sprites/frogbreath_sp/tronco3.png",
    "resources/sprites/frogbreath_sp/tronco4.png"
};

// Caminhos dos sprites para troncos de duas partes
const char *two_part_trunk[] = {
    "resources/sprites/frogbreath_sp/tronco1.png",
    "resources/sprites/frogbreath_sp/tronco4.png"
};

// Estrutura global para armazenar os dados do jogador
struct player player;
Time timer;

// Lista dinâmica para carros ativos
EnemyCar *active_cars = NULL; // Ponteiro para array dinâmico de carros
int active_car_count = 0; // Quantidade de carros ativos
const int max_cars_on_screen = 10; // Máximo de carros na tela
const int lane_y_positions[] = {289, 321, 353, 385, 415}; // Posições Y das faixas dos carros
#define NUM_LANES (sizeof(lane_y_positions) / sizeof(lane_y_positions[0])) // Número de faixas
float spawn_timers[NUM_LANES] = {0}; // Temporizadores de spawn para cada faixa

// Lista dinâmica para troncos ativos
Trunk *trunk = NULL; // Ponteiro para array dinâmico de troncos
int trunk_count = 0; // Quantidade de troncos ativos
const int max_trunk_on_screen = 10; // Máximo de troncos na tela
const int lanetrunk_y_positions[] = {225, 186, 140, 97}; // Posições Y das faixas dos troncos
#define NUM_LANE_TRUNK (sizeof(lanetrunk_y_positions) / sizeof(lanetrunk_y_positions[0])) // Número de faixas de tronco
float trunk_spawn_timers[NUM_LANE_TRUNK] = {0}; // Temporizadores de spawn para cada faixa de tronco

bool paused = false; // Flag de pausa do jogo

int main() {
    // --- Inicialização da janela ---
    const int screen_width = 448;
    const int screen_height = 512;
    InitWindow(screen_width, screen_height, "Frogger 2025"); // Cria a janela do jogo
    SetTargetFPS(60); // Define FPS alvo

    // --- Inicialização do áudio ---
    InitAudioDevice(); // Inicializa o dispositivo de áudio
    if (!IsAudioDeviceReady()) { // Verifica se o áudio foi inicializado corretamente
        printf("Erro: Não foi possível inicializar o dispositivo de áudio\n");
        CloseWindow();
        return 1;
    }
    
    // --- Inicialização de recursos ---
    srand(time(NULL)); // Inicializa o gerador de números aleatórios

    // --- Texturas ---
    Texture2D background = LoadTexture("resources/bg/frogbreath_bg.png"); // Carrega o fundo
    if (background.id == 0) { // Verifica se carregou corretamente
        printf("Erro: Não foi possível carregar a textura 'resources/bg/frogbreath_bg.png'\n");
        CloseWindow();
        return 1;
    }
    Texture2D life = LoadTexture("resources/sprites/life.png"); // Carrega sprite de vida
    Texture2D frog = LoadTexture("resources/sprites/frog.png"); // Carrega sprite do sapo (casas)
    Texture2D fly =  LoadTexture("resources/sprites/fly.png"); // Carrega sprite da mosca

    // --- Sons ---
    Sound effects[4] = {
        LoadSound("resources/sounds/completion.wav"), // Som de completar objetivo
        LoadSound("resources/sounds/death.wav"), // Som de morte
        LoadSound("resources/sounds/jump.wav"), // Som de pulo
        LoadSound("resources/sounds/powerUp.wav") // Som de power up
    };
    SetSoundVolume(effects[0], 0.50f); // Volume do som de completar objetivo
    SetSoundVolume(effects[1], 0.80f); // Volume do som de morte
    SetSoundVolume(effects[2], 0.25f); // Volume do som de pulo
    SetSoundVolume(effects[3], 0.50f); // Volume do som de power up

    // --- Música ---
    Music frogsoath = LoadMusicStream("resources/sounds/frogsoath.ogg"); // Carrega música de fundo

    // --- Fontes ---
    Font font = LoadFontEx("resources/fontes/8_bit_fortress/8-bit-fortress.ttf", 32, NULL, 0); // Carrega fonte customizada
    if (font.baseSize == 0) { // Verifica se carregou corretamente
        printf("Erro: Não foi possível carregar a fonte 8-bit\n");
        CloseWindow();
        return 1;
    }

    // --- Inicialização do jogador ---
    player_init(&player, "resources/sprites/sapo-ani.png", (Vector2){224, 448}); // Inicializa o jogador na posição inicial

    PlayMusicStream(frogsoath); // Começa a tocar a música de fundo

    // --- Loop principal do jogo ---
    while (!WindowShouldClose()) { // Enquanto a janela não for fechada
        UpdateMusicStream(frogsoath); // Atualiza a música de fundo
        float dt = GetFrameTime(); // Tempo decorrido desde o último frame

        if (IsKeyPressed(KEY_ENTER)) { // Alterna pausa ao pressionar ENTER
            paused = !paused;
        }
        
        if (!paused) { // Se não estiver pausado, atualiza o jogo normalmente
            // --- Atualizações normais do jogo ---
            timer_event(&player, dt, font, &timer); // Atualiza o timer do HUD

            // --- Spawn de carros por faixa ---
            for (int i = 0; i < NUM_LANES; i++) {
                spawn_timers[i] -= dt; // Diminui o timer de spawn
                if (spawn_timers[i] <= 0) { // Se chegou a hora de spawnar
                    spawn_timers[i] = rand() % 3 + 1; // Novo tempo aleatório para próximo spawn
                    spawn_car(&active_cars, &active_car_count, car_sprites[i],
                              (i % 2 == 0) ? 100.0f : -120.0f, i, max_cars_on_screen, lane_y_positions); // Spawna carro
                }
            }

            // --- Spawn de troncos por faixa ---
            for (int i = 0; i < NUM_LANE_TRUNK; i++) {
                trunk_spawn_timers[i] -= dt; // Diminui o timer de spawn
                if (trunk_spawn_timers[i] <= 0) { // Se chegou a hora de spawnar
                    trunk_spawn_timers[i] = (rand() % 3) + 2; // Tempo aleatório entre 2 e 4 segundos
                    float speed = (i % 2 == 0) ? 50.0f : -70.0f; // Velocidade do tronco
                    int parts = (rand() % 2 == 0) ? 2 : 4; // Tronco de 2 ou 4 partes
                    const char **sprite_set = (parts == 2) ? two_part_trunk : trunk_sprites; // Escolhe sprites
                    spawn_trunk(&trunk, &trunk_count, sprite_set, speed, i, max_trunk_on_screen, lanetrunk_y_positions, parts); // Spawna tronco
                }
            }
            
            // --- Atualização dos carros ativos ---
            for (int i = 0; i < active_car_count; i++) {
                active_cars[i].update(&active_cars[i], dt); // Atualiza posição do carro
                // Remove carro se saiu da tela
                if ((active_cars[i].speed > 0 && active_cars[i].position.x > GetScreenWidth()) ||
                    (active_cars[i].speed < 0 && active_cars[i].position.x < -active_cars[i].texture.width)) {
                    active_cars[i].unload(&active_cars[i]); // Libera recursos do carro
                    for (int j = i; j < active_car_count - 1; j++) {
                        active_cars[j] = active_cars[j + 1]; // Move carros restantes
                    }
                    active_car_count--;
                    if (active_car_count > 0) {
                        active_cars = realloc(active_cars, active_car_count * sizeof(EnemyCar)); // Redimensiona array
                    } else {
                        free(active_cars); // Libera array se vazio
                        active_cars = NULL;
                    }
                    i--;
                }
            }

            // --- Atualização dos troncos ativos ---
            for (int i = 0; i < trunk_count; i++) {
                trunk[i].update(&trunk[i], dt); // Atualiza posição do tronco
                // Remove tronco se saiu da tela
                if ((trunk[i].speed > 0 && trunk[i].position.x > GetScreenWidth()) ||
                    (trunk[i].speed < 0 && trunk[i].position.x + trunk[i].hitbox.width < 0)) {
                    trunk[i].unload(&trunk[i]); // Libera recursos do tronco
                    for (int j = i; j < trunk_count - 1; j++) {
                        trunk[j] = trunk[j + 1]; // Move troncos restantes
                    }
                    trunk_count--;
                    if (trunk_count > 0) {
                        trunk = realloc(trunk, trunk_count * sizeof(Trunk)); // Redimensiona array
                    } else {
                        free(trunk); // Libera array se vazio
                        trunk = NULL;
                    }
                    i--;
                }
            }
            
            // --- Atualização do jogador ---
            player_update(&player, dt, 32, 32, &effects[2]); // Atualiza o jogador

            Rectangle water_area = {0, 96, (float)GetScreenWidth(), 160}; // Área da água
            bool on_trunk = player_on_trunk(&player, trunk, trunk_count, dt); // Verifica se o jogador está em um tronco

            // Se o jogador está na água, não está em um tronco e não está morto, ele morre afogado
            if (CheckCollisionRecs(player.hitbox, water_area) && !on_trunk && !player.is_dead) {
                player_die(&player, &effects[1]); // Afoga o jogador
            }
            
            // --- Verificação de colisões ---
            if (!player.is_dead && !player.game_over) {
                // Colisão com carros
                for (int i = 0; i < active_car_count; i++) {
                    if (active_cars[i].check_collision(&active_cars[i], player.hitbox)) {
                        player_die(&player, &effects[1]); // Mata o jogador se colidir com carro
                        break;
                    }
                }
            }
        }

        // --- Desenho ---
        BeginDrawing(); // Inicia o desenho do frame
        ClearBackground(RAYWHITE); // Limpa o fundo
        DrawTexture(background, 0, 0, WHITE); // Desenha o fundo

        check_home_event(&player, frog); // Verifica se o jogador chegou em uma casa
        
        if(!paused){
            check_fly_event(&player, fly); // Verifica se o jogador pegou a mosca
        }
        // Troncos
        for (int i = 0; i < trunk_count; i++) {
            trunk[i].draw(&trunk[i]); // Desenha troncos
        }
        
        // Carros
        for (int i = 0; i < active_car_count; i++) {
            active_cars[i].draw(&active_cars[i]); // Desenha carros
        }
        
        // Jogador
        if (!player.is_dead) {
            draw_player(&player, 32, 32, 5); // Desenha o jogador vivo
        } else {
            dead_player(&player, 32, 32, 3); // Desenha animação de morte
        }

        // HUD
        draw_hud(font, player.lives, player.score, life, &timer); // Desenha HUD (vidas, score, timer)

        // --- Tela de Pause ---
        if (paused) {
            pause_game(font); // Desenha tela de pausa
        }

        // --- Tela de Game Over ---
        if (player.game_over) {
            DrawText("GAME OVER", 140, 200, 40, RED); // Mensagem de game over
            DrawText("Pressione ESPAÇO para recomeçar", 80, 260, 20, BLACK); // Instrução para reiniciar
            if (IsKeyPressed(KEY_SPACE)) {
                // Reinicia estado do jogo
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
                
                // Limpa carros e troncos
                free(active_cars);
                active_cars = NULL;
                active_car_count = 0;
                
                free(trunk);
                trunk = NULL;
                trunk_count = 0;

                // Reinicia timers de spawn
                for (int i = 0; i < NUM_LANES; i++) spawn_timers[i] = 0;
                for (int i = 0; i < NUM_LANE_TRUNK; i++) trunk_spawn_timers[i] = 0;
            }
        }

        EndDrawing(); // Finaliza o desenho do frame
    }

    // --- Liberação de recursos ---
    if (active_cars) {
        for (int i = 0; i < active_car_count; i++) active_cars[i].unload(&active_cars[i]); // Libera recursos dos carros
        free(active_cars); // Libera array de carros
    }
    if (trunk) {
        for (int i = 0; i < trunk_count; i++) trunk[i].unload(&trunk[i]); // Libera recursos dos troncos
        free(trunk); // Libera array de troncos
    }

    UnloadTexture(background); // Libera textura do fundo
    UnloadTexture(life); // Libera textura de vida
    UnloadTexture(frog); // Libera textura do sapo
    UnloadTexture(fly); // Libera textura da mosca
    
    for (int i = 0; i < 4; i++) UnloadSound(effects[i]); // Libera sons
    UnloadMusicStream(frogsoath); // Libera música
    
    player_unload(&player); // Libera recursos do jogador
    UnloadFont(font); // Libera fonte

    CloseAudioDevice(); // Fecha dispositivo de áudio
    CloseWindow(); // Fecha a janela

    return 0;
}
