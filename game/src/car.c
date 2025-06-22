#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdbool.h>

#include "car.h"

// ===================== INICIALIZAÇÃO =====================
static void enemy_init(struct EnemyCar *self, const char *texture_path, Vector2 position, float speed) {
    self->texture = LoadTexture(texture_path);
    if (self->texture.id == 0) {
        TraceLog(LOG_ERROR, "Erro ao carregar textura do inimigo : %s", texture_path);
        exit(1);
    }
    self->position = position;
    self->speed = speed;
    self->active = true;
    self->hitbox = (Rectangle) {
        .x = position.x,
        .y = position.y,
        .width = self->texture.width,
        .height = self->texture.height
    };
}

// ===================== ATUALIZAÇÃO =====================
static void enemy_update(struct EnemyCar *self, float dt) {
    self->position.x += self->speed * dt;
    if (self->speed > 0 && self->position.x > GetScreenWidth()) {
        self->position.x = -self->texture.width;
    } else if (self->speed < 0 && self->position.x < -self->texture.width) {
        self->position.x = GetScreenWidth();
    }
    self->hitbox.x = self->position.x;
    self->hitbox.y = self->position.y;
}

// ===================== DESENHO =====================
static void enemy_draw(const struct EnemyCar *self) {
    DrawTexture(self->texture, self->position.x, self->position.y, WHITE);
}

// ===================== LIBERAÇÃO DE RECURSOS =====================
static void enemy_unload(struct EnemyCar *self) {
    UnloadTexture(self->texture);
}

// ===================== COLISÃO =====================
static bool enemy_check_collision(const struct EnemyCar *self, Rectangle player_hitbox) {
    return CheckCollisionRecs(self->hitbox, player_hitbox);
}

// Verifica se há espaço livre na faixa para spawnar um novo carro
bool is_lane_free(EnemyCar *active_cars, int active_car_count, int lane_index, float x_position, float car_width) {
    for (int i = 0; i < active_car_count; i++) {
        if (active_cars[i].position.y == lane_index) {
            if (fabs(active_cars[i].position.x - x_position) < car_width) {
                return false;
            }
        }
    }
    return true;
}

// ===================== SPAWN DE CARRO =====================
void spawn_car(EnemyCar **active_cars, int *active_car_count, const char *sprite_path, float speed, int lane_index, int max_cars_on_screen, const int *lane_y_positions) {
    if (*active_car_count >= max_cars_on_screen) return;

    float x = (speed > 0) ? -64 : GetScreenWidth() + 64;
    if (!is_lane_free(*active_cars, *active_car_count, lane_y_positions[lane_index], x, 64)) return;

    *active_cars = realloc(*active_cars, (*active_car_count + 1) * sizeof(EnemyCar));
    if (!*active_cars) {
        TraceLog(LOG_ERROR, "Erro ao alocar memória para os carros!");
        CloseWindow();
        exit(1);
    }

    (*active_cars)[*active_car_count] = create_enemy_car();
    (*active_cars)[*active_car_count].init(&(*active_cars)[*active_car_count], sprite_path, (Vector2){x, lane_y_positions[lane_index]}, speed);
    (*active_car_count)++;
}

// ===================== CRIAÇÃO DE ENEMY =====================
EnemyCar create_enemy_car() {
    EnemyCar enemy = {
        .init = enemy_init,
        .update = enemy_update,
        .draw = enemy_draw,
        .unload = enemy_unload,
        .check_collision = enemy_check_collision
    };
    return enemy;
}
