#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "animation.h"
#include <stdbool.h>

struct player {
    Vector2 position;
    Vector2 target_position;
    Texture2D texture;
    int score;
    Animation anim;
    bool is_moving;
    float rotation;
    Rectangle hitbox;
};

void player_init(struct player *p, const char *texture_path, Vector2 position);
void player_update(struct player *p, float dt, int frame_width, int frame_height);
void draw_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row);
void player_unload(struct player *p);

#endif
