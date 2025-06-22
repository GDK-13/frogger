#ifndef CAR_H
#define CAR_H

#include "raylib.h"
#include <stdbool.h>

// ===================== ESTRUTURA DO CARRO INIMIGO =====================
typedef struct EnemyCar {
    Vector2 position;
    float speed;
    Texture2D texture;
    Rectangle hitbox;
    bool active;

    // Métodos associados ao inimigo
    void (*init)(struct EnemyCar *self, const char *texture_path, Vector2 position, float speed);
    void (*update)(struct EnemyCar *self, float dt);
    void (*draw)(const struct EnemyCar *self);
    void (*unload)(struct EnemyCar *self);
    bool (*check_collision)(const struct EnemyCar *self, Rectangle player_hitbox);
} EnemyCar;

// ===================== FUNÇÕES DE CONTROLE =====================

// Cria e retorna um novo EnemyCar
EnemyCar create_enemy_car();

// Verifica se a faixa está livre para spawn
bool is_lane_free(EnemyCar *active_cars, int active_car_count, int lane_index, float x_position, float car_width);

// Spawna um novo carro inimigo
void spawn_car(EnemyCar **active_cars, int *active_car_count, const char *sprite_path, float speed, int lane_index, int max_cars_on_screen, const int *lane_y_positions);

#endif
