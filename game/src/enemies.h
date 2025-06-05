#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"

typedef struct Enemy {
    Vector2 position;
    int speed;
    Texture2D texture;
} Enemy;


// Atualiza a posição dos inimigos
void UpdateEnemies(Enemy enemies[], bool left_right, int i);

// Desenha os inimigos na tela
void DrawEnemies(Enemy enemies[]);

#endif