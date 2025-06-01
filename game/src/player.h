#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

extern Vector2 playerPosition;

//faz o player andar 
void UpdatePlayer(void);

//desenha o player olhando pras direções e recebe a textura dele como parametro;
void DrawPlayer(Texture2D player);

#endif
