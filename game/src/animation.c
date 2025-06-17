#include "raylib.h"
#include "animation.h"

// Atualiza o estado da animação com base no tempo decorrido
void animation_update(Animation *anim, float delta_time) {
    anim->duration_left -= delta_time; // Reduz o tempo restante para o frame atual
    if (anim->duration_left <= 0) {
        anim->current_frame++; // Avança para o próximo frame
        if (anim->current_frame > anim->last_frame) {
            // Verifica o tipo de animação para decidir o comportamento ao atingir o último frame
            switch (anim->type) {
            case REPEATING:
                anim->current_frame = anim->first_frame; // Reinicia a animação
                break;
            case ONESHOT:
                anim->current_frame = anim->last_frame; // Mantém no último frame
                break;
            default:
                break;
            }
        }
        anim->duration_left = anim->speed; // Reinicia o tempo do frame
    }
}

// Retorna o retângulo correspondente ao frame atual da animação
Rectangle animation_frame_rect(const Animation *anim, int frame_width, int frame_height, int num_frames_per_row) {
    int frame = anim->current_frame; // Obtém o frame atual
    int row = frame / num_frames_per_row; // Calcula a linha do frame na textura
    int col = frame % num_frames_per_row; // Calcula a coluna do frame na textura

    // Retorna o retângulo correspondente ao frame atual
    return (Rectangle){
        .x = col * frame_width,
        .y = row * frame_height,
        .width = frame_width,
        .height = frame_height
    };
}
