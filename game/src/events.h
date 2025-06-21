#ifndef EVENTS_H
#define EVENTS_H

#include "raylib.h"
#include "player.h"

typedef struct {
    int minutes;      // Minutos do cronômetro
    int seconds;      // Segundos do cronômetro
    float elapsed;    // Tempo total decorrido em segundos
} Time;

// Função que gerencia o cronômetro do jogo
void timer_event(struct player *p, float delta_time, Font font, Time *time);

// Função que gerencia as casas do topo e eventos de chegada
void check_home_event(struct player *p, Texture2D sapo);

// Função que gerencia o evento de pegar a mosca
void check_fly_event(struct player *p, Texture2D fly);

#endif