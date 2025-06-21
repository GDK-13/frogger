#ifndef HUD_H
#define HUD_H

#include "raylib.h" // Necessário para o tipo Font e funções de desenho
#include "events.h" // Necessário para a estrutura Time

// Função que desenha o HUD do jogo (vidas e score)
// font: fonte a ser usada para o texto
// vidas: número de vidas do jogador
// score: pontuação atual do jogador
void draw_hud(Font font, int vidas, int score, Texture2D life, Time *timer);

#endif