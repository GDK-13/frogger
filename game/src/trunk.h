#ifndef TRUNK_H
#define TRUNK_H

#include "raylib.h"       // Biblioteca principal para gráficos e entrada
#include <stdbool.h>      // Biblioteca para manipulação de valores booleanos

#define MAX_PARTS 4 // Número máximo de partes de um tronco

// Estrutura que representa um tronco
typedef struct Trunk {
    Vector2 position;    // Posição atual do tronco
    float speed;         // Velocidade do tronco
    Texture2D textures[MAX_PARTS];   // Vetor de texturas para cada parte do tronco
    Rectangle hitbox;    // Hitbox para colisão
    bool active;         // Indica se o tronco está ativo
    int parts; // Quantidade de partes do tronco
    float parts_width; // Largura de cada parte
    int lane; // Faixa em que o tronco está

    // Métodos da TAD 
    void (*init)(struct Trunk *self, const char **texture_paths, Vector2 position, float speed, int parts); // Inicializa o tronco
    void (*update)(struct Trunk *self, float dt); // Atualiza o estado hitbox do tronco
    void (*draw)(const struct Trunk  *self); // Desenha o tronco na tela
    void (*unload)(struct Trunk *self); // Libera os recursos do tronco
} Trunk;

// Função para criar um novo tronco
Trunk create_trunk();

// Funções adicionais para controle dos troncos
void spawn_trunk(Trunk **trunk, int *trunk_count, const char **sprite_paths, float speed, int lane_index, int max_trunk_on_screen, const int *lane_y_positions, int parts); // Spawn de um novo tronco
bool is_trunk_free(Trunk *trunk, int trunk_count, int lane_y, int new_x, int width); 

#endif