#ifndef CAR_H
#define CAR_H

#include "raylib.h"       // Biblioteca principal para gráficos e entrada
#include <stdbool.h>      // Biblioteca para manipulação de valores booleanos

// Estrutura que representa um carro inimigo
typedef struct EnemyCar {
    Vector2 position;    // Posição atual do inimigo
    float speed;         // Velocidade do inimigo
    Texture2D texture;   // Textura do inimigo
    Rectangle hitbox;    // Hitbox para colisão
    bool active;         // Indica se o inimigo está ativo ou não

    // Métodos da TAD (funções associadas ao inimigo)
    void (*init)(struct EnemyCar *self, const char *texture_path, Vector2 position, float speed); // Inicializa o inimigo
    void (*update)(struct EnemyCar *self, float dt); // Atualiza o estado do inimigo
    void (*draw)(const struct EnemyCar *self); // Desenha o inimigo na tela
    void (*unload)(struct EnemyCar *self); // Libera os recursos do inimigo
    bool (*check_collision)(const struct EnemyCar *self, Rectangle player_hitbox); // Verifica colisão com o jogador
} EnemyCar;

// Função para criar um novo inimigo
EnemyCar create_enemy_car();

// Funções adicionais para controle de inimigos
bool is_lane_free(EnemyCar *active_cars, int active_car_count, int lane_index, float x_position, float car_width); // Verifica se a faixa está livre
void spawn_car(EnemyCar **active_cars, int *active_car_count, const char *sprite_path, float speed, int lane_index, int max_cars_on_screen, const int *lane_y_positions); // Spawn de um novo carro

#endif
