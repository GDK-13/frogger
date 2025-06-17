#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"

// Enum para definir o tipo de animação
typedef enum {
    REPEATING = 1, // Animação que repete continuamente
    ONESHOT = 2,   // Animação que executa uma única vez
} AnimationType;

// Estrutura para armazenar os dados de uma animação
typedef struct {
    int first_frame;      // Primeiro frame da animação
    int last_frame;       // Último frame da animação
    int current_frame;    // Frame atual sendo exibido
    float duration_left;  // Tempo restante para o frame atual
    float speed;          // Velocidade da animação (tempo por frame)
    AnimationType type;   // Tipo da animação (REPEATING ou ONESHOT)
} Animation;

// Atualiza o estado da animação com base no tempo decorrido
void animation_update(Animation *anim, float delta_time);

// Retorna o retângulo correspondente ao frame atual da animação
Rectangle animation_frame_rect(const Animation *anim, int frame_width, int frame_height, int num_frames_per_row);

#endif
