#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"

typedef enum {
    REPEATING = 1,
    ONESHOT = 2,
} AnimationType;

typedef struct {
    int first_frame;
    int last_frame;
    int current_frame;
    float duration_left;
    float speed;
    AnimationType type;
} Animation;

void animation_update(Animation *anim, float delta_time);
Rectangle animation_frame_rect(const Animation *anim, int frame_width, int frame_height, int num_frames_per_row);

#endif
