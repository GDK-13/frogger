#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"
#include <stdbool.h> 

struct enemyCar {
    Vector2 position;    // Posição atual do inimigo
    float speed;         // Velocidade do inimigo
    Texture2D texture;   // Textura do inimigo 
    Rectangle hitbox;    // Hitbox para colisão
    bool active;         // Se está ativo ou não no jogo
};

// Inicializa os dados do inimigo
void enemy_init(struct enemyCar *e, const char *texture_path, Vector2 position, float speed);

//Atualiza o estado do inimigo
void enemy_update(struct enemyCar *e, float dt);

//Desenha o inimigo na tela
void enemy_draw(const struct enemyCar *e);

//Libera os recursos do inimigo
void enemy_unload(struct enemyCar *e);

// Verifica colisões com o jogador
bool enemy_check_collision(const struct enemyCar *e, Rectangle player_hitbox);

#endif
