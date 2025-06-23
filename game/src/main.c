#include "raylib.h"
#include "player.h"
#include "car.h"
#include "trunk.h"
#include "events.h"
#include "animation.h"
#include "hud.h"
#include "screen.h"
#include "turtle.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// ===================== SPRITES E CONSTANTES =====================
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

const char *turtle_sprite = "resources/sprites/frogbreath_sp/tartaruga.png";

// ===================== VARIÁVEIS GLOBAIS =====================
struct player player;
Time timer;

EnemyCar *active_cars = NULL;
int active_car_count = 0;
const int max_cars_on_screen = 10;
const int lane_y_positions[] = {289, 321, 353, 385, 415};
#define NUM_LANES (sizeof(lane_y_positions) / sizeof(lane_y_positions[0]))
float spawn_timers[NUM_LANES] = {0};

Trunk *trunk = NULL;
int trunk_count = 0;
const int max_trunk_on_screen = 15;
const int lanetrunk_y_positions[] = {198, 126, 96};
#define NUM_LANE_TRUNK (sizeof(lanetrunk_y_positions) / sizeof(lanetrunk_y_positions[0]))
float trunk_spawn_timers[NUM_LANE_TRUNK] = {0};

Turtle *turtle = NULL;
int turtle_count = 0;
const int max_turtles_on_screen = 20;
const int turtle_lane_y_positions[] = {225, 156};
#define NUM_TURTLE_LANES (sizeof(turtle_lane_y_positions) / sizeof(turtle_lane_y_positions[0]))
float turtle_spawn_timers[NUM_TURTLE_LANES] = {0};

// ===================== FUNÇÃO DE RESET =====================
void reset_game_state() {
    extern float game_timer;
    extern bool timer_running;
    extern bool winner;
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

    for (int i = 0; i < 5; i++) player.oc_houses[i] = false;

    free(active_cars);
    active_cars = NULL;
    active_car_count = 0;

    free(trunk);
    trunk = NULL;
    trunk_count = 0;

    free(turtle);
    turtle = NULL;
    turtle_count = 0;

    for (int i = 0; i < NUM_LANES; i++) spawn_timers[i] = 0;
    for (int i = 0; i < NUM_LANE_TRUNK; i++) trunk_spawn_timers[i] = 0;
    for (int i = 0; i < NUM_TURTLE_LANES; i++) turtle_spawn_timers[i] = 0;

    game_timer = 0.0f;      // <-- Zera o timer global
    timer_running = true;   // <-- Garante que o timer volte a contar
    winner = false;         // <-- Reseta o estado de vitória
}

// ===================== MAIN =====================
int main() {
    // Inicialização da janela e do Raylib
    const int screen_width = 448;
    const int screen_height = 512;
    InitWindow(screen_width, screen_height, "Frogger 2025");
    SetTargetFPS(60);

    ScreenManager screenManager = create_screen_manager();
    screenManager.init(&screenManager);

    // Inicialização do áudio
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        printf("Erro: Não foi possível inicializar o dispositivo de áudio\n");
        CloseWindow();
        return 1;
    }

    srand(time(NULL));

    // Carregamento de texturas e sons
    Texture2D background = LoadTexture("resources/bg/frogbreath_bg.png");
    if (background.id == 0) {
        printf("Erro: Não foi possível carregar textura do background\n");
        CloseWindow();
        return 1;
    }
    Texture2D life = LoadTexture("resources/sprites/life.png");
    Texture2D frog = LoadTexture("resources/sprites/frog.png");
    Texture2D fly = LoadTexture("resources/sprites/fly.png");

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

    Music frogsoath = LoadMusicStream("resources/sounds/frogsoath.ogg");

    // Inicialização do jogador
    player_init(&player, "resources/sprites/sapo-ani.png", (Vector2){224, 448});

    PlayMusicStream(frogsoath);

    bool paused = false;
    bool show_hitboxes = false;

    // ===================== LOOP PRINCIPAL =====================
    while (!WindowShouldClose()) {
        UpdateMusicStream(frogsoath);
        float dt = GetFrameTime();

        // Alterna exibição das hitboxes
        if (IsKeyPressed(KEY_F4)) show_hitboxes = !show_hitboxes;

        GameScreen previousScreen = screenManager.current;
        screenManager.update(&screenManager, &screenManager.current);

        // Controle de pausa
        if (screenManager.current == SCREEN_GAMEPLAY && IsKeyPressed(KEY_ENTER)) {
            screenManager.current = SCREEN_PAUSE;
        } else if (screenManager.current == SCREEN_PAUSE && IsKeyPressed(KEY_ENTER)) {
            screenManager.current = SCREEN_GAMEPLAY;
        }

        // Atualiza cronômetro apenas durante gameplay
        if (screenManager.current == SCREEN_GAMEPLAY) {
            timer_event(&player, dt, screenManager.font, &timer);
        }

        // Reset do jogo ao sair do menu para gameplay
        if (previousScreen == SCREEN_MENU && screenManager.current == SCREEN_GAMEPLAY) {
            reset_game_state();
        }

        // ===================== GAMEPLAY =====================
        if (screenManager.current == SCREEN_GAMEPLAY) {
            // Spawn de carros
            for (int i = 0; i < NUM_LANES; i++) {
                spawn_timers[i] -= dt;
                if (spawn_timers[i] <= 0) {
                    spawn_timers[i] = (rand() % 2) + 3;
                    spawn_car(&active_cars, &active_car_count, car_sprites[i],
                              (i % 2 == 0) ? 90.0f : -100.0f, i, max_cars_on_screen, lane_y_positions);
                }
            }

            // Spawn de troncos
            for (int i = 0; i < NUM_LANE_TRUNK; i++) {
                trunk_spawn_timers[i] -= dt;
                if (trunk_spawn_timers[i] <= 0) {
                    trunk_spawn_timers[i] = (rand() % 2) + 3;
                    float speed = (i % 2 == 0) ? 30.0f : -45.0f;
                    int parts = (rand() % 2 == 0) ? 2 : 4;
                    const char **sprite_set = (parts == 2) ? two_part_trunk : trunk_sprites;
                    spawn_trunk(&trunk, &trunk_count, sprite_set, speed, i, max_trunk_on_screen, lanetrunk_y_positions, parts);
                }
            }

            // Spawn de tartarugas
            for (int i = 0; i < NUM_TURTLE_LANES; i++) {
                turtle_spawn_timers[i] -= dt;
                if (turtle_spawn_timers[i] <= 0) {
                    turtle_spawn_timers[i] = (rand() % 2) + 3;
                    float speed = (i % 2 == 0) ? 45.0f : -60.0f;
                    int parts = 3;
                    spawn_turtle(&turtle, &turtle_count, turtle_sprite, speed, i, max_turtles_on_screen, turtle_lane_y_positions, parts);
                }
            }

            // Atualiza e remove carros/troncos/tartarugas fora da tela
            for (int i = 0; i < active_car_count; i++) {
                active_cars[i].update(&active_cars[i], dt);
                if ((active_cars[i].speed > 0 && active_cars[i].position.x > GetScreenWidth()) ||
                    (active_cars[i].speed < 0 && active_cars[i].position.x < -active_cars[i].texture.width)) {
                    active_cars[i].unload(&active_cars[i]);
                    for (int j = i; j < active_car_count - 1; j++) active_cars[j] = active_cars[j + 1];
                    active_car_count--;
                    if (active_car_count > 0) active_cars = realloc(active_cars, active_car_count * sizeof(EnemyCar));
                    else { free(active_cars); active_cars = NULL; }
                    i--;
                }
            }
            for (int i = 0; i < trunk_count; i++) {
                trunk[i].update(&trunk[i], dt);
                if ((trunk[i].speed > 0 && trunk[i].position.x > GetScreenWidth()) ||
                    (trunk[i].speed < 0 && trunk[i].position.x + trunk[i].hitbox.width < 0)) {
                    trunk[i].unload(&trunk[i]);
                    for (int j = i; j < trunk_count - 1; j++) trunk[j] = trunk[j + 1];
                    trunk_count--;
                    if (trunk_count > 0) trunk = realloc(trunk, trunk_count * sizeof(Trunk));
                    else { free(trunk); trunk = NULL; }
                    i--;
                }
            }
            for (int i = 0; i < turtle_count; i++) {
                turtle[i].update(&turtle[i], dt);
                if ((turtle[i].speed > 0 && turtle[i].position.x > GetScreenWidth()) ||
                    (turtle[i].speed < 0 && turtle[i].position.x + turtle[i].hitbox.width < 0)) {
                    turtle[i].unload(&turtle[i]);
                    for (int j = i; j < turtle_count - 1; j++) turtle[j] = turtle[j + 1];
                    turtle_count--;
                    if (turtle_count > 0) turtle = realloc(turtle, turtle_count * sizeof(Turtle));
                    else { free(turtle); turtle = NULL; }
                    i--;
                }
            }

            // Atualiza jogador
            player_update(&player, dt, 32, 32, &effects[2]);

            // Checa se o jogador está na água sem apoio
            Rectangle water_area = {0, 96, (float)GetScreenWidth(), 155};
            bool on_trunk = player_on_trunk(&player, trunk, trunk_count, dt);
            bool on_turtle = player_on_turtle(&player, turtle, turtle_count, dt);
            if (CheckCollisionRecs(player.hitbox, water_area) && !on_trunk && !on_turtle && !player.is_dead) {
                player_die(&player, &effects[1]);
            }

            // Checa se o jogador saiu da tela
            if (!player.is_dead) {
                Rectangle bounds = player.hitbox;
                if (bounds.x + bounds.width < 0 || bounds.x > GetScreenWidth() ||
                    bounds.y + bounds.height < 0 || bounds.y > GetScreenHeight()) {
                    player_die(&player, &effects[1]);
                }
            }

            // Colisão com carros
            if (!player.is_dead && !player.game_over) {
                for (int i = 0; i < active_car_count; i++) {
                    if (active_cars[i].check_collision(&active_cars[i], player.hitbox)) {
                        player_die(&player, &effects[1]);
                        break;
                    }
                }
            }

            // Checa vitória
            bool victory = true;
            for (int i = 0; i < 5; i++) {
                if (!player.oc_houses[i]) {
                    victory = false;
                    break;
                }
            }
            if (victory && !player.game_over && !player.is_dead || IsKeyDown(KEY_F3)) {
                screenManager.final_score = player.score;
                screenManager.total_time = timer.elapsed;
                screenManager.current = SCREEN_WIN;
            }

            // Checa game over
            if (player.game_over || IsKeyPressed(KEY_F2)) {
                screenManager.current = SCREEN_GAME_OVER;
            }
        }

        // ===================== DESENHO =====================
        BeginDrawing();
        ClearBackground(BLACK);

        if (screenManager.current == SCREEN_GAMEPLAY) {
            ClearBackground(RAYWHITE);
            DrawTexture(background, 0, 0, WHITE);

            check_home_event(&player, frog, show_hitboxes, &effects[0]);
            check_fly_event(&player, fly, show_hitboxes);

            for (int i = 0; i < trunk_count; i++) {
                trunk[i].draw(&trunk[i]);
                if (show_hitboxes)
                    DrawRectangleLines((int)trunk[i].hitbox.x, (int)trunk[i].hitbox.y,
                                      (int)trunk[i].hitbox.width, (int)trunk[i].hitbox.height, ORANGE);
            }
            for (int i = 0; i < active_car_count; i++) {
                active_cars[i].draw(&active_cars[i]);
                if (show_hitboxes)
                    DrawRectangleLines((int)active_cars[i].hitbox.x, (int)active_cars[i].hitbox.y,
                                      (int)active_cars[i].hitbox.width, (int)active_cars[i].hitbox.height, RED);
            }
            for (int i = 0; i < turtle_count; i++) {
                turtle[i].draw(&turtle[i]);
                if (show_hitboxes)
                    DrawRectangleLines((int)turtle[i].hitbox.x, (int)turtle[i].hitbox.y,
                                      (int)turtle[i].hitbox.width, (int)turtle[i].hitbox.height, DARKGREEN);
            }
            if (!player.is_dead) {
                draw_player(&player, 32, 32, 5);
            } else {
                dead_player(&player, 32, 32, 3);
            }
            if (show_hitboxes)
                DrawRectangleLines((int)player.hitbox.x, (int)player.hitbox.y,
                                   (int)player.hitbox.width, (int)player.hitbox.height, BLUE);

            draw_hud(screenManager.font, player.lives, player.score, life, &timer);
        }

        // Desenha telas de menu, pausa, vitória e game over
        screenManager.draw(&screenManager);

        EndDrawing();
    }

    // ===================== LIBERAÇÃO DE RECURSOS =====================
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
    UnloadTexture(screenManager.backmenu);
    UnloadTexture(screenManager.backui);
    UnloadTexture(screenManager.imgstart);
    UnloadTexture(screenManager.imgstarthover);
    UnloadTexture(screenManager.imgrestart);
    UnloadTexture(screenManager.imgbacktomenu);
    UnloadTexture(screenManager.imgbacktomenuhover);
    UnloadTexture(screenManager.imgquit);
    UnloadTexture(screenManager.imgquithover);

    UnloadTexture(turtle->texture);

    for (int i = 0; i < trunk_count; i++) {
        for (int j = 0; j < trunk[i].parts; j++) {
            UnloadTexture(trunk[i].textures[j]);
        }
    

    for (int i = 0; i < 4; i++) UnloadSound(effects[i]);
    UnloadMusicStream(frogsoath);

    player_unload(&player);
    UnloadFont(screenManager.font);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
}
