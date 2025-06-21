#include "events.h"
#include "raylib.h"


#define FLY_DURATION 15.0f    // Tempo que a mosca fica ativa
#define FLY_COOLDOWN 20.0f    // Tempo de cooldown para reaparecer

float game_timer = 0.0f;      // Tempo total do jogo em segundos
bool timer_running = true;    // Indica se o cronômetro está ativo

Vector2 home[5] = {
        { 22, 92 },
        { 22 + (3 * 32), 92 },
        { 22 + (6 * 32), 92 },
        { 22 + (9 * 32), 92 },
        { 22 + (12 * 32), 92 }
    };

int fly_index = 0;
bool fly_active = false;




void timer_event(struct player *p, float delta_time, Font fonte, Time *timer) {
    if (timer_running) {
        game_timer += delta_time; // Incrementa o tempo do jogo
        if (p->game_over == true) { // Marca o jogo como terminado
            timer_running = false; // Para o cronômetro
        }
    }
    // Desenha o cronômetro no canto superior direito
    timer->elapsed = game_timer; // Atualiza o tempo total decorrido
    timer->minutes = (int)(game_timer / 60);
    timer->seconds = (int)(game_timer) % 60;

}

void check_home_event(struct player *p, Texture2D sapo) {
    extern int fly_index;
    extern bool fly_active;

    for (int i = 0; i < 5; i++) {
        DrawRectangleLines(home[i].x, home[i].y, 20, 32, RED);
        if (p->oc_houses[i]) {
            DrawTexture(sapo, home[i].x - 6 , home[i].y - 28, WHITE);
        }
    }

    for (int i = 0; i < 5; i++) {
        if (CheckCollisionRecs(p->hitbox, (Rectangle){ home[i].x, home[i].y, 20, 32 })) {
            if (IsKeyPressed(KEY_UP) && !p->oc_houses[i]) {
                // Verifica se a casa tem mosca ativa
                if (fly_active && fly_index == i) {
                    p->score += 100; // Pontuação extra pela mosca
                    fly_active = false; // Mosca desaparece
                }
                p->score += 200;
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

void check_fly_event(struct player *p, Texture2D fly) {
    static int fly_index_local = 0;           // Casa onde a mosca está
    static float fly_timer = FLY_DURATION;    // Tempo de vida da mosca
    static bool fly_active_local = false;     // Se a mosca está ativa
    static float fly_cooldown = FLY_COOLDOWN; // Cooldown para o próximo aparecimento

    // Torna as variáveis acessíveis externamente para check_home_event
    fly_index = fly_index_local;
    fly_active = fly_active_local;

    // Atualiza timers
    float dt = GetFrameTime();
    if (fly_active_local) {
        fly_timer -= dt;
    } else {
        fly_cooldown -= dt;
    }

    // Sorteia nova posição apenas em casas vazias, após cooldown
    if ((!fly_active_local && fly_cooldown <= 0.0f) || (fly_active_local && fly_timer <= 0.0f)) {
        if (!fly_active_local) {
            // Só sorteia se cooldown acabou e não está ativa
            int tentativas = 0;
            do {
                fly_index_local = GetRandomValue(0, 4);
                tentativas++;
            } while (p->oc_houses[fly_index_local] && tentativas < 10); // Evita loop infinito se todas ocupadas

            fly_timer = FLY_DURATION; // Usa a constante definida
            fly_active_local = true;
        } else if (fly_timer <= 0.0f) {
            // Mosca some, inicia cooldown
            fly_active_local = false;
            fly_cooldown = FLY_COOLDOWN; // Usa a constante definida
        }
    }

    // Só desenha se a casa estiver vazia
    if (!p->oc_houses[fly_index_local] && fly_active_local) {
        // Posição da mosca
        Vector2 fly_position = {
            home[fly_index_local].x - 6,
            home[fly_index_local].y - 28
        };

        Rectangle fly_hitbox = { home[fly_index_local].x, home[fly_index_local].y, 20, 32 };

        DrawTexture(fly, fly_position.x, fly_position.y, WHITE);
        DrawRectangleLines(fly_hitbox.x, fly_hitbox.y, fly_hitbox.width, fly_hitbox.height, GREEN);
    }

    // Atualiza as variáveis globais para uso em check_home_event
    fly_index = fly_index_local;
    fly_active = fly_active_local;
}



