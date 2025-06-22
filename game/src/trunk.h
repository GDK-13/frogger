#ifndef TRUNK_H
#define TRUNK_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_PARTS 4

// ===================== ESTRUTURA DO TRONCO =====================
typedef struct Trunk {
    Vector2 position;
    float speed;
    Texture2D textures[MAX_PARTS];
    Rectangle hitbox;
    bool active;
    int parts;
    float parts_width;
    int lane;

    // Métodos do tronco
    void (*init)(struct Trunk *self, const char **texture_paths, Vector2 position, float speed, int parts);
    void (*update)(struct Trunk *self, float dt);
    void (*draw)(const struct Trunk *self);
    void (*unload)(struct Trunk *self);
} Trunk;

// ===================== FUNÇÕES DE CONTROLE =====================

// Cria e retorna um novo tronco
Trunk create_trunk(void);

// Spawna um novo tronco
void spawn_trunk(Trunk **trunk, int *trunk_count, const char **sprite_paths, float speed, int lane_index, int max_trunk_on_screen, const int *lane_y_positions, int parts);

// Verifica se a faixa está livre para spawnar tronco
bool is_trunk_free(Trunk *trunk, int trunk_count, int lane_y, int new_x, int width);



#endif