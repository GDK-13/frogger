#ifndef TURTLE_H
#define TURTLE_H

#include "raylib.h"
#include <stdbool.h>

// ===================== ESTRUTURA DA TARTARUGA =====================
typedef struct Turtle {
    Vector2 position;
    float speed;
    Texture2D texture;
    Rectangle hitbox;
    bool active;
    int lane;
    int parts;

    // Métodos da tartaruga
    void (*init)(struct Turtle *self, const char *texture_path, Vector2 position, float speed, int parts);
    void (*update)(struct Turtle *self, float dt);
    void (*draw)(const struct Turtle *self);
    void (*unload)(struct Turtle *self);
} Turtle;

// ===================== FUNÇÕES DE CONTROLE =====================

// Cria e retorna uma nova tartaruga
Turtle create_turtle(void);

// Spawna uma nova tartaruga
void spawn_turtle(Turtle **turtle, int *turtle_count, const char *sprite_path, float speed, int lane_index, int max_turtle_on_screen, const int *lane_y_positions, int parts);

// Verifica se a faixa está livre para spawnar tartaruga
bool is_turtle_free(Turtle *turtle, int turtle_count, int lane_y, int new_x, int width);

#endif