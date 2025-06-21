#ifndef EVENTS_H
#define EVENTS_H

#include "raylib.h"
#include "player.h"

// Estrutura para armazenar o tempo do jogo
typedef struct {
    int minutes;    // Minutos decorridos
    int seconds;    // Segundos decorridos
    float elapsed;  // Tempo total em segundos
} Time;

// Atualiza o cronômetro do jogo
void timer_event(struct player *p, float delta_time, Font fonte, Time *timer);

// Gerencia e desenha as casas do topo e a pontuação ao chegar nelas
void check_home_event(struct player *p, Texture2D sapo);

// Gerencia e desenha a mosca bônus
void check_fly_event(struct player *p, Texture2D fly);


#endif