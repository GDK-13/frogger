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

// Caminhos dos sprites dos carros usados no jogo
const char *car_sprites[] = {
    "resources/sprites/frogbreath_sp/caminhao.png",
    "resources/sprites/frogbreath_sp/carro1.png",
    "resources/sprites/frogbreath_sp/carro2.png",
    "resources/sprites/frogbreath_sp/carro3.png",
    "resources/sprites/frogbreath_sp/carro4.png"
};

// Caminhos dos sprites dos troncos usados no jogo
const char *trunk_sprites[] = {
    "resources/sprites/frogbreath_sp/tronco1.png",
    "resources/sprites/frogbreath_sp/tronco2.png",
    "resources/sprites/frogbreath_sp/tronco3.png",
    "resources/sprites/frogbreath_sp/tronco4.png"
};

// Sprites para troncos de duas partes (especiais)
const char *two_part_trunk[] = {
    "resources/sprites/frogbreath_sp/tronco1.png",
    "resources/sprites/frogbreath_sp/tronco4.png"
};

// Estado global do jogador
struct player player;
Time timer;

// Listas dinâmicas para inimigos e obstáculos
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

bool paused = false;

// Função que reseta o estado do jogo para o início
void reset_game_state() {
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

    // Libera memória e reseta listas de carros, troncos e tartarugas
    free(active_cars);
    active_cars = NULL;
    active_car_count = 0;

    free(trunk);
    trunk = NULL;
    trunk_count = 0;

    free(turtle);
    turtle = NULL;
    turtle_count = 0;

    // Zera timers de spawn
    for (int i = 0; i < NUM_LANES; i++) spawn_timers[i] = 0;
    for (int i = 0; i < NUM_LANE_TRUNK; i++) trunk_spawn_timers[i] = 0;
    for (int i = 0; i < NUM_TURTLE_LANES; i++) turtle_spawn_timers[i] = 0;
}

int main() {
    // Inicializa a janela do jogo
    const int screen_width = 448;
    const int screen_height = 512;
    InitWindow(screen_width, screen_height, "Frogger 2025");
    SetTargetFPS(60);

    ScreenManager screenManager = create_screen_manager();
    screenManager.init(&screenManager);

    // Inicializa áudio
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        printf("Erro: Não foi possível inicializar o dispositivo de áudio\n");
        CloseWindow();
        return 1;
    }

    srand(time(NULL));  // Inicializa semente do gerador de números aleatórios

    // Carrega texturas
    Texture2D background = LoadTexture("resources/bg/frogbreath_bg.png");
    if (background.id == 0) {
        printf("Erro: Não foi possível carregar textura do background\n");
        CloseWindow();
        return 1;
    }

    Texture2D life = LoadTexture("resources/sprites/life.png");
    Texture2D frog = LoadTexture("resources/sprites/frog.png");
    Texture2D fly = LoadTexture("resources/sprites/fly.png");

    // Carrega efeitos sonoros
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

    // Carrega música de fundo
    Music frogsoath = LoadMusicStream("resources/sounds/frogsoath.ogg");

    // Inicializa jogador com sprite e posição inicial
    player_init(&player, "resources/sprites/sapo-ani.png", (Vector2){224, 448});

    PlayMusicStream(frogsoath);

    // Loop principal do jogo
    while (!WindowShouldClose()) {
        UpdateMusicStream(frogsoath);
        float dt = GetFrameTime();

        // Salva a tela anterior
        GameScreen previousScreen = screenManager.current;

        // Atualiza a tela (trata cliques, botões, etc.)
        screenManager.update(&screenManager, &screenManager.current);

        // Alterna pausa com Enter
        if (IsKeyPressed(KEY_ENTER)) {
            paused = !paused;
        }

        // Atualiza o cronômetro se não estiver pausado e estiver em gameplay
        if (!paused && screenManager.current == SCREEN_GAMEPLAY) {
            timer_event(&player, dt, screenManager.font , &timer);
        }

        if (previousScreen == SCREEN_MENU && screenManager.current == SCREEN_GAMEPLAY) {
            reset_game_state();
        }

        if (screenManager.current == SCREEN_GAMEPLAY) {
            // Spawn de carros nas faixas
            for (int i = 0; i < NUM_LANES; i++) {
                spawn_timers[i] -= dt;
                if (spawn_timers[i] <= 0) {
                    spawn_timers[i] = (rand() % 2) + 3;
                    spawn_car(&active_cars, &active_car_count, car_sprites[i],
                              (i % 2 == 0) ? 90.0f : -100.0f, i, max_cars_on_screen, lane_y_positions);
                }
            }

            // Spawn de troncos nas faixas
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

            // Spawn de tartarugas nas faixas
            for (int i = 0; i < NUM_TURTLE_LANES; i++) {
                turtle_spawn_timers[i] -= dt;
                if (turtle_spawn_timers[i] <= 0) {
                    turtle_spawn_timers[i] = (rand() % 3) + 5;
                    float speed = (i % 2 == 0) ? 45.0f : -60.0f;
                    int parts = 3;
                    spawn_turtle(&turtle, &turtle_count, "resources/sprites/frogbreath_sp/tartaruga.png", speed, i, max_turtles_on_screen, turtle_lane_y_positions, parts);
                }
            }

            // Atualiza carros ativos e remove os que saem da tela
            for (int i = 0; i < active_car_count; i++) {
                active_cars[i].update(&active_cars[i], dt);

                if ((active_cars[i].speed > 0 && active_cars[i].position.x > GetScreenWidth()) ||
                    (active_cars[i].speed < 0 && active_cars[i].position.x < -active_cars[i].texture.width)) {
                    active_cars[i].unload(&active_cars[i]);

                    // Remove carro da lista
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

            // Atualiza troncos ativos e remove os que saem da tela
            for (int i = 0; i < trunk_count; i++) {
                trunk[i].update(&trunk[i], dt);

                if ((trunk[i].speed > 0 && trunk[i].position.x > GetScreenWidth()) ||
                    (trunk[i].speed < 0 && trunk[i].position.x + trunk[i].hitbox.width < 0)) {
                    trunk[i].unload(&trunk[i]);

                    // Remove tronco da lista
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

            // Atualiza tartarugas ativas e remove as que saem da tela
            for (int i = 0; i < turtle_count; i++) {
                turtle[i].update(&turtle[i], dt);

                if ((turtle[i].speed > 0 && turtle[i].position.x > GetScreenWidth()) ||
                    (turtle[i].speed < 0 && turtle[i].position.x + turtle[i].hitbox.width < 0)) {
                    turtle[i].unload(&turtle[i]);

                    // Remove tartaruga da lista
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

            // Atualiza o jogador (movimento, animação, etc)
            player_update(&player, dt, 32, 32, &effects[2]);

            // Define área da água para detectar afogamento
            Rectangle water_area = {0, 96, (float)GetScreenWidth(), 160};

            // Verifica se jogador está sobre tronco ou tartaruga
            bool on_trunk = player_on_trunk(&player, trunk, trunk_count, dt);
            bool on_turtle = player_on_turtle(&player, turtle, turtle_count, dt);

            // Se estiver na água sem apoio e vivo, o jogador morre afogado
            if (CheckCollisionRecs(player.hitbox, water_area) && !on_trunk && !on_turtle && !player.is_dead) {
                player_die(&player, &effects[1]);
            }

            // Verifica colisão do jogador com carros
            if (!player.is_dead && !player.game_over) {
                for (int i = 0; i < active_car_count; i++) {
                    if (active_cars[i].check_collision(&active_cars[i], player.hitbox)) {
                        player_die(&player, &effects[1]);
                        break;
                    }
                }
            }

            // Verifica condição de vitória (casas ocupadas)
            bool victory = true;
            for (int i = 0; i < 5; i++) {
                if (!player.oc_houses[i]) {
                    victory = false;
                    break;
                }
            }

            // Se venceu, muda para tela de vitória e pausa o jogo
            if (victory && !player.game_over && !player.is_dead) {
                screenManager.final_score = player.score;
                screenManager.total_time = timer.elapsed;
                screenManager.current = SCREEN_WIN;
                paused = true;
            }
        }

        // Começa desenho na tela
        BeginDrawing();

        ClearBackground(BLACK);

        screenManager.update(&screenManager, &screenManager.current);
        screenManager.draw(&screenManager);



        if (screenManager.current == SCREEN_GAMEPLAY) {

             ClearBackground(RAYWHITE);
            // Desenha background do jogo
            DrawTexture(background, 0, 0, WHITE);

            // Desenha elementos da tela atual
            screenManager.draw(&screenManager);

            // Eventos do jogo
            check_home_event(&player, frog);

            if (!paused) {
                check_fly_event(&player, fly);
            }

            // Desenha troncos e suas hitboxes
            for (int i = 0; i < trunk_count; i++) {
                trunk[i].draw(&trunk[i]);
                DrawRectangleLines((int)trunk[i].hitbox.x, (int)trunk[i].hitbox.y,
                                   (int)trunk[i].hitbox.width, (int)trunk[i].hitbox.height, ORANGE);
            }

            // Desenha carros e suas hitboxes
            for (int i = 0; i < active_car_count; i++) {
                active_cars[i].draw(&active_cars[i]);
                DrawRectangleLines((int)active_cars[i].hitbox.x, (int)active_cars[i].hitbox.y,
                                   (int)active_cars[i].hitbox.width, (int)active_cars[i].hitbox.height, RED);
            }

            // Desenha tartarugas e suas hitboxes
            for (int i = 0; i < turtle_count; i++) {
                turtle[i].draw(&turtle[i]);
                DrawRectangleLines((int)turtle[i].hitbox.x, (int)turtle[i].hitbox.y,
                                   (int)turtle[i].hitbox.width, (int)turtle[i].hitbox.height, DARKGREEN);
            }

            // Desenha jogador, diferente se morto ou vivo
            if (!player.is_dead) {
                draw_player(&player, 32, 32, 5);
            } else {
                dead_player(&player, 32, 32, 3);
            }

            // Desenha hitbox do jogador
            DrawRectangleLines((int)player.hitbox.x, (int)player.hitbox.y,
                               (int)player.hitbox.width, (int)player.hitbox.height, BLUE);

            // Desenha HUD com informações (vidas, pontuação, etc)
            draw_hud(screenManager.font, player.lives, player.score, life, &timer);

            // Tela de pausa
            if (paused) {
                pause_game(screenManager.font);

                const char *buttonText = "Voltar ao Menu";
                int buttonFontSize = 20;
                int textWidth = MeasureText(buttonText, buttonFontSize);
                int x = (screen_width - textWidth) / 2;
                int y = 300;
                int padding = 10;

                Rectangle menuButton = { x - padding, y - padding, textWidth + 2 * padding, buttonFontSize + 2 * padding };
                Vector2 mouse = GetMousePosition();

                DrawRectangleRec(menuButton, LIGHTGRAY);
                DrawText(buttonText, x, y, buttonFontSize, BLACK);

                // Detecta clique no botão para voltar ao menu
                if (CheckCollisionPointRec(mouse, menuButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    screenManager.current = SCREEN_MENU;
                    paused = false;
                }
            }

            // Tela de Game Over
            if (player.game_over) {
                DrawRectangle(0, 0, screen_width, screen_height, (Color){0, 0, 0, 180});

                Vector2 center = {
                    (screen_width - MeasureTextEx( screenManager.font, "GAME OVER", 40, 0).x) / 2, 200
                };
                DrawTextEx(screenManager.font, "GAME OVER", center, 40, 0, RED);

                const char *subtitle = "RESTART";
                int font_size = 20;
                int text_width = MeasureText(subtitle, font_size);
                int text_height = font_size;
                int x = (screen_width - text_width) / 2;
                int y = 260;

                // Retângulo do botão Restart
                Rectangle buttonBounds = { x - 10, y - 5, text_width + 20, text_height + 10 };
                Vector2 mousePoint = GetMousePosition();
                Color buttonColor = GRAY;

                DrawRectangleRec(buttonBounds, buttonColor);
                DrawText(subtitle, x, y, font_size, WHITE);

                // Detecta clique no botão Restart para resetar o jogo
                if (CheckCollisionPointRec(mousePoint, buttonBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    reset_game_state();
                }
            } else {
                // Desenha outros elementos da tela caso não seja Game Over
                screenManager.draw(&screenManager);
            }
        }
        EndDrawing();
    }
         
    
    // Libera todos os recursos antes de fechar o programa
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
    UnloadFont(screenManager.font);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
