#include "raylib.h"
#include "animation.h"

// ===================== ATUALIZAÇÃO DA ANIMAÇÃO =====================
void animation_update(Animation *anim, float delta_time) {
    anim->duration_left -= delta_time;
    if (anim->duration_left <= 0) {
        anim->current_frame++;
        if (anim->current_frame > anim->last_frame) {
            // Comportamento ao atingir o último frame
            switch (anim->type) {
            case REPEATING:
                anim->current_frame = anim->first_frame;
                break;
            case ONESHOT:
                anim->current_frame = anim->last_frame;
                break;
            default:
                break;
            }
        }
        anim->duration_left = anim->speed;
    }
}

// ===================== FRAME ATUAL DA ANIMAÇÃO =====================
Rectangle animation_frame_rect(const Animation *anim, int frame_width, int frame_height, int num_frames_per_row) {
    int frame = anim->current_frame;
    int row = frame / num_frames_per_row;
    int col = frame % num_frames_per_row;

    return (Rectangle){
        .x = col * frame_width,
        .y = row * frame_height,
        .width = frame_width,
        .height = frame_height
    };
}
