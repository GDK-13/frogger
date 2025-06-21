#ifndef TURTLE_H
#define TURTLE_H

#include "raylib.h"       // Biblioteca principal para gráficos e entrada
#include <stdbool.h>      // Biblioteca para manipulação de valores booleanos

// Estrutura que representa um tronco
typedef struct Turtle {
    Vector2 position;    // Posição atual da tartaruga
    float speed;         // Velocidade da tartaruga
    Texture2D texture;    // Textura da tartaruga
    Rectangle hitbox;    // Hitbox para colisão
    bool active;         // Indica se o tartaruga está ativo
    int lane; // Faixa em que a tartaruga está
    int parts;

    // Métodos da TAD 
    void (*init)(struct Turtle *self, const char *texture_path, Vector2 position, float speed, int parts); // Inicializa a tartaruga
    void (*update)(struct Turtle *self, float dt); // Atualiza o estado hitbox da tartaruga
    void (*draw)(const struct Turtle  *self); // Desenha a tartaruga na tela
    void (*unload)(struct Turtle *self); // Libera os recursos da tartaruga
} Turtle;

// Função para criar uma nova tartaruga
Turtle create_turtle();

// Funções adicionais para controle dos troncos
void spawn_turtle(Turtle **turtle, int *turtle_count, const char *sprite_path, float speed, int lane_index, int max_turtle_on_screen, const int *lane_y_positions, int parts); // Spawn de um nova tartaruga
bool is_turtle_free(Turtle *turtle, int turtle_count, int lane_y, int new_x, int width); 

#endif