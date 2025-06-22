#include "raylib.h"
#include "player.h"
#include "car.h"
#include "trunk.h"
#include "events.h"
#include "animation.h"
#include "hud.h"
#include "turtle.h"
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
EnemyCar *active_cars = NULL;
int active_car_count = 0;
const int max_cars_on_screen = 10;
const int lane_y_positions[] = {289, 321, 353, 385, 415};
#define NUM_LANES (sizeof(lane_y_positions) / sizeof(lane_y_positions[0]))
float spawn_timers[NUM_LANES] = {0};

// Lista dinâmica para troncos ativos
Trunk *trunk = NULL;
int trunk_count = 0;
const int max_trunk_on_screen = 10;
const int lanetrunk_y_positions[] = {198, 126, 96};
#define NUM_LANE_TRUNK (sizeof(lanetrunk_y_positions) / sizeof(lanetrunk_y_positions[0]))
float trunk_spawn_timers[NUM_LANE_TRUNK] = {0};

bool paused = false;

Turtle *turtle = NULL;
int turtle_count = 0;
const int max_turtles_on_screen = 10;
const int turtle_lane_y_positions[] = {225, 156};
#define NUM_TURTLE_LANES (sizeof(turtle_lane_y_positions) / sizeof(turtle_lane_y_positions[0]))
float turtle_spawn_timers[NUM_TURTLE_LANES] = {0};

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
    
    srand(time(NULL));

    // --- Texturas ---
    Texture2D background = LoadTexture("resources/bg/frogbreath_bg.png");
    if (background.id == 0) {
        printf("Erro: Não foi possível carregar a textura 'resources/bg/frogbreath_bg.png'\n");
        CloseWindow();
        return 1;
    }
    Texture2D life = LoadTexture("resources/sprites/life.png");
    Texture2D frog = LoadTexture("resources/sprites/frog.png");
    Texture2D fly =  LoadTexture("resources/sprites/fly.png");

    // --- Sons ---
    Sound effects[4] = {
        LoadSound("resources/sounds/completion.wav"),
        LoadSound("resources/sounds/death.wav"),
        LoadSound("resources/sounds/jump.wav"),
        LoadSound("resources/sounds/powerUp.wav")
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

    PlayMusicStream(frogsoath);

    // --- Loop principal do jogo ---
    while (!WindowShouldClose()) {
        UpdateMusicStream(frogsoath);
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_ENTER)) {
            paused = !paused;
        }
        
        if (!paused) {
            timer_event(&player, dt, font, &timer);

            // --- Spawn de carros por faixa ---
            for (int i = 0; i < NUM_LANES; i++) {
                spawn_timers[i] -= dt;
                if (spawn_timers[i] <= 0) {
                    spawn_timers[i] = rand() % 3 + 1;
                    spawn_car(&active_cars, &active_car_count, car_sprites[i],
                              (i % 2 == 0) ? 100.0f : -120.0f, i, max_cars_on_screen, lane_y_positions);
                }
            }

            // --- Spawn de troncos por faixa ---
            for (int i = 0; i < NUM_LANE_TRUNK; i++) {
                trunk_spawn_timers[i] -= dt;
                if (trunk_spawn_timers[i] <= 0) {
                    trunk_spawn_timers[i] = (rand() % 3) + 2;
                    float speed = (i % 2 == 0) ? 50.0f : -70.0f;
                    int parts = (rand() % 2 == 0) ? 2 : 4;
                    const char **sprite_set = (parts == 2) ? two_part_trunk : trunk_sprites;
                    spawn_trunk(&trunk, &trunk_count, sprite_set, speed, i, max_trunk_on_screen, lanetrunk_y_positions, parts);
                }
            }

            // --- Spawn de tartarugas por faixa ---
            for (int i = 0; i < NUM_TURTLE_LANES; i++) {
                turtle_spawn_timers[i] -= dt;
                if (turtle_spawn_timers[i] <= 0) {
                    turtle_spawn_timers[i] = (rand() % 3) + 4;
                    float speed = (i % 2 == 0) ? 45.0f : -60.0f;
                    int parts = 3;
                    spawn_turtle(&turtle, &turtle_count, "resources/sprites/frogbreath_sp/tartaruga.png", speed, i, max_turtles_on_screen, turtle_lane_y_positions, parts);
                }
            }
            
            // --- Atualização dos carros ativos ---
            for (int i = 0; i < active_car_count; i++) {
                active_cars[i].update(&active_cars[i], dt);
                if ((active_cars[i].speed > 0 && active_cars[i].position.x > GetScreenWidth()) ||
                    (active_cars[i].speed < 0 && active_cars[i].position.x < -active_cars[i].texture.width)) {
                    active_cars[i].unload(&active_cars[i]);
                    for (int j = i; j < active_car_count - 1; j++) {
                        active_cars[j] = active_cars[j + 1];
                    }
                    active_car_count--;
                    if (active_car_count > 0) {
                        active_cars = realloc(active_cars, active_car_count * sizeof(EnemyCar));
                    } else {
                        free(active_cars);
                        active_cars = NULL;
                    }
                    i--;
                }
            }

            // --- Atualização dos troncos ativos ---
            for (int i = 0; i < trunk_count; i++) {
                trunk[i].update(&trunk[i], dt);
                if ((trunk[i].speed > 0 && trunk[i].position.x > GetScreenWidth()) ||
                    (trunk[i].speed < 0 && trunk[i].position.x + trunk[i].hitbox.width < 0)) {
                    trunk[i].unload(&trunk[i]);
                    for (int j = i; j < trunk_count - 1; j++) {
                        trunk[j] = trunk[j + 1];
                    }
                    trunk_count--;
                    if (trunk_count > 0) {
                        trunk = realloc(trunk, trunk_count * sizeof(Trunk));
                    } else {
                        free(trunk);
                        trunk = NULL;
                    }
                    i--;
                }
            }

            // --- Atualização das tartarugas ---
            for (int i = 0; i < turtle_count; i++) {
                turtle[i].update(&turtle[i], dt);
                if ((turtle[i].speed > 0 && turtle[i].position.x > GetScreenWidth()) ||
                    (turtle[i].speed < 0 && turtle[i].position.x + turtle[i].hitbox.width < 0)) {
                    turtle[i].unload(&turtle[i]);
                    for (int j = i; j < turtle_count - 1; j++) {
                        turtle[j] = turtle[j + 1];
                    }
                    turtle_count--;
                    if (turtle_count > 0) {
                        turtle = realloc(turtle, turtle_count * sizeof(Turtle));
                    } else {
                        free(turtle);
                        turtle = NULL;
                    }
                    i--;
                }
            }

            // --- Atualização do jogador ---
            player_update(&player, dt, 32, 32, &effects[2]);

            // Área da água e checagem de tronco/tartaruga
            Rectangle water_area = {0, 96, (float)GetScreenWidth(), 160};
            bool on_trunk = player_on_trunk(&player, trunk, trunk_count, dt);
            bool on_turtle = player_on_turtle(&player, turtle, turtle_count, dt);

            // Se o jogador está na água, não está em um tronco/tartaruga e não está morto, ele morre afogado
            if (CheckCollisionRecs(player.hitbox, water_area) && !on_trunk && !on_turtle && !player.is_dead) {
                player_die(&player, &effects[1]);
            }

            // --- Verificação de colisões com carros ---
            if (!player.is_dead && !player.game_over) {
                for (int i = 0; i < active_car_count; i++) {
                    if (active_cars[i].check_collision(&active_cars[i], player.hitbox)) {
                        player_die(&player, &effects[1]);
                        break;
                    }
                }
            }
        }

        // --- Desenho ---
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(background, 0, 0, WHITE);

        check_home_event(&player, frog);

        if (!paused) {
            check_fly_event(&player, fly);
        }

        // Troncos
        for (int i = 0; i < trunk_count; i++) {
            trunk[i].draw(&trunk[i]);
            // Desenha hitbox do tronco
            DrawRectangleLines(
                (int)trunk[i].hitbox.x,
                (int)trunk[i].hitbox.y,
                (int)trunk[i].hitbox.width,
                (int)trunk[i].hitbox.height,
                ORANGE
            );
        }
        // Carros
        for (int i = 0; i < active_car_count; i++) {
            active_cars[i].draw(&active_cars[i]);
            // Desenha hitbox do carro
            DrawRectangleLines(
                (int)active_cars[i].hitbox.x,
                (int)active_cars[i].hitbox.y,
                (int)active_cars[i].hitbox.width,
                (int)active_cars[i].hitbox.height,
                RED
            );
        }
        // Tartarugas
        for (int i = 0; i < turtle_count; i++) {
            turtle[i].draw(&turtle[i]);
            // Desenha hitbox da tartaruga
            DrawRectangleLines(
                (int)turtle[i].hitbox.x,
                (int)turtle[i].hitbox.y,
                (int)turtle[i].hitbox.width,
                (int)turtle[i].hitbox.height,
                DARKGREEN
            );
        }
        // Jogador
        if (!player.is_dead) {
            draw_player(&player, 32, 32, 5);
        } else {
            dead_player(&player, 32, 32, 3);
        }
        // Desenha hitbox do jogador
        DrawRectangleLines(
            (int)player.hitbox.x,
            (int)player.hitbox.y,
            (int)player.hitbox.width,
            (int)player.hitbox.height,
            BLUE
        );

        // HUD
        draw_hud(font, player.lives, player.score, life, &timer);

        // --- Tela de Pause ---
        if (paused) {
            pause_game(font);
        }

        // --- Tela de Game Over ---
        if (player.game_over) {
            DrawText("GAME OVER", 140, 200, 40, RED);
            DrawText("Pressione ESPAÇO para recomeçar", 80, 260, 20, BLACK);
            if (IsKeyPressed(KEY_SPACE)) {
                // Reinicia estado do jogo
                player.game_over = false;
                player.lives = 5;
                player.score = 0;
                player.final_score = 0;
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
                free(turtle);
                turtle = NULL;
                turtle_count = 0;
                // Reinicia timers de spawn
                for (int i = 0; i < NUM_LANES; i++) spawn_timers[i] = 0;
                for (int i = 0; i < NUM_LANE_TRUNK; i++) trunk_spawn_timers[i] = 0;
                for (int i = 0; i < NUM_TURTLE_LANES; i++) turtle_spawn_timers[i] = 0;
            }
        }

        EndDrawing();
    }

    // --- Liberação de recursos ---
    if (active_cars) {
        for (int i = 0; i < active_car_count; i++) active_cars[i].unload(&active_cars[i]);
        free(active_cars);
    }
    if (trunk) {
        for (int i = 0; i < trunk_count; i++) trunk[i].unload(&trunk[i]);
        free(trunk);
    }
    if (turtle) {
        for (int i = 0; i < turtle_count; i++) turtle[i].unload(&turtle[i]);
        free(turtle);
    }

    UnloadTexture(background);
    UnloadTexture(life);
    UnloadTexture(frog);
    UnloadTexture(fly);
    for (int i = 0; i < 4; i++) UnloadSound(effects[i]);
    UnloadMusicStream(frogsoath);
    player_unload(&player);
    UnloadFont(font);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
