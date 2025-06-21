#include "events.h"
#include "raylib.h"

// Define o tempo de duração da mosca e o cooldown entre aparições
#define FLY_DURATION 15.0f    // Tempo que a mosca fica ativa (em segundos)
#define FLY_COOLDOWN 20.0f    // Tempo de cooldown para reaparecer (em segundos)

// Variáveis globais para controle do cronômetro
float game_timer = 0.0f;      // Tempo total do jogo em segundos
bool timer_running = true;    // Indica se o cronômetro está ativo

// Posições das casas no topo da tela
Vector2 home[5] = {
        { 22, 92 },
        { 22 + (3 * 32), 92 },
        { 22 + (6 * 32), 92 },
        { 22 + (9 * 32), 92 },
        { 22 + (12 * 32), 92 }
    };

// Variáveis globais para controle da mosca bônus
int fly_index = 0;    // Índice da casa onde a mosca está
bool fly_active = false; // Indica se a mosca está ativa

// Atualiza o cronômetro do jogo e armazena o tempo decorrido em 'timer'
void timer_event(struct player *p, float delta_time, Font fonte, Time *timer) {
    if (timer_running) {
        game_timer += delta_time; // Incrementa o tempo do jogo
        if (p->game_over == true) { // Se o jogo acabou, para o cronômetro
            timer_running = false;
        }
    }
    // Atualiza os campos da struct Time
    timer->elapsed = game_timer;
    timer->minutes = (int)(game_timer / 60);
    timer->seconds = (int)(game_timer) % 60;
}

// Desenha as casas do topo, os sapos ocupando as casas e gerencia a pontuação ao chegar nelas
void check_home_event(struct player *p, Texture2D sapo) {
    extern int fly_index;
    extern bool fly_active;

    // Desenha as casas e os sapos ocupando as casas
    for (int i = 0; i < 5; i++) {
        DrawRectangleLines(home[i].x, home[i].y, 20, 32, RED); // Desenha o contorno da casa
        if (p->oc_houses[i]) {
            DrawTexture(sapo, home[i].x - 6 , home[i].y - 28, WHITE); // Desenha o sapo na casa ocupada
        }
    }

    // Verifica se o jogador chegou em uma casa livre
    for (int i = 0; i < 5; i++) {
        if (CheckCollisionRecs(p->hitbox, (Rectangle){ home[i].x, home[i].y, 20, 32 })) {
            if (IsKeyPressed(KEY_UP) && !p->oc_houses[i]) {
                // Se a casa tem mosca ativa, adiciona pontos extras e remove a mosca
                if (fly_active && fly_index == i) {
                    p->score += 100; // Pontuação extra pela mosca
                    fly_active = false; // Mosca desaparece
                }
                // Pontuação normal por ocupar a casa
                p->score += 200;
                // Reinicia o jogador na posição inicial
                p->position = p->start_position;
                p->target_position = p->start_position;
                p->anim.current_frame = 0;
                p->rotation = 0.0f;
                p->is_dead = false;
                p->oc_houses[i] = true; // Marca a casa como ocupada
            }
        }
    }
}

// Gerencia o evento da mosca bônus: sorteio, timers, desenho e colisão
void check_fly_event(struct player *p, Texture2D fly) {
    // Variáveis estáticas para manter o estado da mosca entre frames
    static int fly_index_local = 0;           // Casa onde a mosca está
    static float fly_timer = FLY_DURATION;    // Tempo de vida da mosca
    static bool fly_active_local = false;     // Se a mosca está ativa
    static float fly_cooldown = FLY_COOLDOWN; // Cooldown para o próximo aparecimento

    // Atualiza as variáveis globais para uso em check_home_event
    fly_index = fly_index_local;
    fly_active = fly_active_local;

    // Atualiza timers da mosca e cooldown
    float dt = GetFrameTime();
    if (fly_active_local) {
        fly_timer -= dt; // Diminui o tempo de vida da mosca
    } else {
        fly_cooldown -= dt; // Diminui o cooldown
    }

    // Sorteia nova posição para a mosca apenas em casas vazias, após cooldown
    if ((!fly_active_local && fly_cooldown <= 0.0f) || (fly_active_local && fly_timer <= 0.0f)) {
        if (!fly_active_local) {
            // Sorteia uma casa vazia para a mosca aparecer
            int tentativas = 0;
            do {
                fly_index_local = GetRandomValue(0, 4);
                tentativas++;
            } while (p->oc_houses[fly_index_local] && tentativas < 10); // Evita loop infinito se todas ocupadas

            fly_timer = FLY_DURATION; // Reinicia o tempo de vida da mosca
            fly_active_local = true;  // Ativa a mosca
        } else if (fly_timer <= 0.0f) {
            // Mosca some, inicia cooldown
            fly_active_local = false;
            fly_cooldown = FLY_COOLDOWN; // Reinicia o cooldown
        }
    }

    // Só desenha a mosca se a casa estiver vazia e a mosca estiver ativa
    if (!p->oc_houses[fly_index_local] && fly_active_local) {
        // Calcula a posição da mosca
        Vector2 fly_position = {
            home[fly_index_local].x - 6,
            home[fly_index_local].y - 28
        };

        // Define a hitbox da mosca para debug/colisão visual
        Rectangle fly_hitbox = { home[fly_index_local].x, home[fly_index_local].y, 20, 32 };

        // Desenha a mosca e sua hitbox
        DrawTexture(fly, fly_position.x, fly_position.y, WHITE);
        DrawRectangleLines(fly_hitbox.x, fly_hitbox.y, fly_hitbox.width, fly_hitbox.height, GREEN);
    }

    // Atualiza as variáveis globais para uso em check_home_event
    fly_index = fly_index_local;
    fly_active = fly_active_local;
}



