#include "events.h"
#include "raylib.h"

// ===================== DEFINIÇÕES E VARIÁVEIS GLOBAIS =====================
#define FLY_DURATION 15.0f
#define FLY_COOLDOWN 20.0f
#define MAX_TIME_SCORE 180.0f

float game_timer = 0.0f;
bool timer_running = true;
bool winner = false;

Vector2 home[5] = {
    { 22, 92 },
    { 22 + (3 * 32), 92 },
    { 22 + (6 * 32), 92 },
    { 22 + (9 * 32), 92 },
    { 22 + (12 * 32), 92 }
};

int fly_index = 0;
bool fly_active = false;

// ===================== CRONÔMETRO E PONTUAÇÃO FINAL =====================
void timer_event(struct player *p, float delta_time, Font fonte, Time *timer) {
    if (timer_running) {
        game_timer += delta_time;
        if (p->game_over) timer_running = false;
    }
    timer->elapsed = game_timer;
    timer->minutes = (int)(game_timer / 60);
    timer->seconds = (int)(game_timer) % 60;
    if (p->game_over || winner) {
        p->final_score = p->score;
        float total_time = timer->minutes * 60 + timer->seconds;
        float time_bonus = MAX_TIME_SCORE - total_time;
        if (time_bonus < 0) time_bonus = 0;
        p->final_score += (int)(time_bonus * 5);
    }
}

// ===================== EVENTO DAS CASAS DO TOPO =====================
void check_home_event(struct player *p, Texture2D sapo, bool show_hitboxes, Sound *effects) {
    extern int fly_index;
    extern bool fly_active;

    for (int i = 0; i < 5; i++) {
        if (show_hitboxes)
            DrawRectangleLines(home[i].x, home[i].y, 20, 32, RED);
        if (p->oc_houses[i])
            DrawTexture(sapo, home[i].x - 6 , home[i].y - 28, WHITE);
    }

    for (int i = 0; i < 5; i++) {
        if (CheckCollisionRecs(p->hitbox, (Rectangle){ home[i].x, home[i].y, 20, 32 })) {
            if (IsKeyPressed(KEY_UP) && !p->oc_houses[i]) {
                if (fly_active && fly_index == i) {
                    p->score += 100;
                    fly_active = false;
                }
                p->score += 200;
                p->position = p->start_position;
                p->target_position = p->start_position;
                p->anim.current_frame = 0;
                p->rotation = 0.0f;
                p->is_dead = false;
                p->oc_houses[i] = true;
                PlaySound(effects[0]);
            }
        }
    }
    if (p->oc_houses[0] && p->oc_houses[1] && p->oc_houses[2] && p->oc_houses[3] && p->oc_houses[4]) {
        winner = true;
        timer_running = false;
    }
}

// ===================== EVENTO DA MOSCA BÔNUS =====================
void check_fly_event(struct player *p, Texture2D fly, bool show_hitboxes) {
    static int fly_index_local = 0;
    static float fly_timer = FLY_DURATION;
    static bool fly_active_local = false;
    static float fly_cooldown = FLY_COOLDOWN;

    fly_index = fly_index_local;
    fly_active = fly_active_local;

    float dt = GetFrameTime();
    if (fly_active_local) {
        fly_timer -= dt;
    } else {
        fly_cooldown -= dt;
    }

    if ((!fly_active_local && fly_cooldown <= 0.0f) || (fly_active_local && fly_timer <= 0.0f)) {
        if (!fly_active_local) {
            int tentativas = 0;
            do {
                fly_index_local = GetRandomValue(0, 4);
                tentativas++;
            } while (p->oc_houses[fly_index_local] && tentativas < 10);
            fly_timer = FLY_DURATION;
            fly_active_local = true;
        } else if (fly_timer <= 0.0f) {
            fly_active_local = false;
            fly_cooldown = FLY_COOLDOWN;
        }
    }

    if (!p->oc_houses[fly_index_local] && fly_active_local) {
        Vector2 fly_position = {
            home[fly_index_local].x - 6,
            home[fly_index_local].y - 28
        };
        Rectangle fly_hitbox = { home[fly_index_local].x, home[fly_index_local].y, 20, 32 };
        DrawTexture(fly, fly_position.x, fly_position.y, WHITE);
        if (show_hitboxes)
            DrawRectangleLines(fly_hitbox.x, fly_hitbox.y, fly_hitbox.width, fly_hitbox.height, GREEN);
    }

    fly_index = fly_index_local;
    fly_active = fly_active_local;
}



