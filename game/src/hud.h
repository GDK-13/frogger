#ifndef HUD_H
#define HUD_H

#include "raylib.h"
#include "events.h"

// ===================== HUD =====================

// Desenha o HUD do jogo (vidas, score e timer)
// font: fonte a ser usada para o texto
// vidas: número de vidas do jogador
// score: pontuação atual do jogador
void draw_hud(Font font, int vidas, int score, Texture2D life, Time *timer);

#endif