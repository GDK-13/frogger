#ifndef EVENTS_H
#define EVENTS_H

#include "raylib.h"
#include "player.h"

// ===================== ESTRUTURA DE TEMPO =====================
typedef struct {
    int minutes;
    int seconds;
    float elapsed;
} Time;

// ===================== FUNÇÕES DE EVENTOS =====================

// Atualiza o cronômetro do jogo
void timer_event(struct player *p, float delta_time, Font fonte, Time *timer);

// Gerencia e desenha as casas do topo e a pontuação ao chegar nelas
void check_home_event(struct player *p, Texture2D sapo, bool show_hitboxes, Sound *effects);

// Gerencia e desenha a mosca bônus
void check_fly_event(struct player *p, Texture2D fly, bool show_hitboxes);


#endif