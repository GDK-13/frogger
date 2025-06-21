#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdbool.h>

#include "player.h"
#include "animation.h"
#include "trunk.h"

bool oc_houses[5] = { false, false, false, false, false };

void player_init(struct player *p, const char *texture_path, Vector2 position) {
    p->start_position = position;
    p->target_position = position;
    p->position = position;

    p->texture = LoadTexture(texture_path);
    if (p->texture.id == 0) {
        TraceLog(LOG_ERROR, "Erro ao carregar sprite do jogador: %s", texture_path);
        exit(1);
    }

    p->lives = 5;
    p->score = 0;
    p->is_moving = false;
    p->rotation = 0.0f;
    p->is_dead = false;
    p->game_over = false;
    p->death_timer = 0.0f;
    p->min_y_position = position.y;

    p->hitbox = (Rectangle){
        .x = position.x,
        .y = position.y,
        .width = p->texture.width / 5,
        .height = p->texture.height / 1
    };

    p->anim = (Animation){
        .first_frame = 0, .last_frame = 4, .current_frame = 0,
        .duration_left = 0.1f, .speed = 0.1f, .type = ONESHOT
    };

    p->death_texture = LoadTexture("resources/sprites/morte.png");
    p->death_animation = (Animation) {
        .first_frame = 0, .last_frame = 2, .current_frame = 0,
        .duration_left = 0.1f, .speed = 0.1f, .type = ONESHOT
    };

    for (int i = 0; i < 5; i++) {
        p->oc_houses[i] = false;
    }
}

void player_update(struct player *p, float dt, int frame_width, int frame_height, Sound *jump_sound) {
    if (p->is_dead) {
        animation_update(&p->death_animation, dt);
        p->death_timer += dt;
        p->hitbox = (Rectangle){0, 0, 0, 0}; // Zera a Hitbox para evitar colisões fantasma

        if (!p->game_over && p->death_timer >= 2.0f) {
            p->is_dead = false;
            p->position = p->start_position;
            p->target_position = p->start_position;
            p->anim.current_frame = 0;
            p->rotation = 0.0f;
        }
        return;
    }

    static float cooldown = 0.0f;
    if (cooldown > 0.0f) {
        cooldown -= dt;
    }

    if (p->is_moving) {
        p->position = Vector2Lerp(p->position, p->target_position, dt * 17.0f);
        animation_update(&p->anim, dt);

        if (Vector2Distance(p->position, p->target_position) < 0.1f) {
            p->position = p->target_position;
            p->is_moving = false;
            p->anim.current_frame = p->anim.first_frame;
            p->anim.duration_left = p->anim.speed;
            cooldown = 0.01f;
        }
    }

    if (!p->is_moving && cooldown <= 0.0f) {
        Vector2 direction = {0.0f, 0.0f};
        if (IsKeyPressed(KEY_RIGHT)) {
            direction.x = frame_width; p->rotation = 90.0f; PlaySound(*jump_sound);
        } else if (IsKeyPressed(KEY_LEFT)) {
            direction.x = -frame_width; p->rotation = 270.0f; PlaySound(*jump_sound);
        } else if (IsKeyPressed(KEY_UP)) {
            direction.y = -frame_height; p->rotation = 0.0f; PlaySound(*jump_sound);
        } else if (IsKeyPressed(KEY_DOWN)) {
            direction.y = frame_height; p->rotation = 180.0f; PlaySound(*jump_sound);
        }

        if (direction.x != 0.0f || direction.y != 0.0f) {
            Vector2 new_target_position = Vector2Add(p->position, direction);
            if (new_target_position.x >= 0 && new_target_position.x + frame_width <= GetScreenWidth() &&
                new_target_position.y >= 96 && new_target_position.y + frame_height <= GetScreenHeight()) {
                p->target_position = new_target_position;
                p->is_moving = true;
                p->anim.current_frame = p->anim.first_frame;
                p->anim.duration_left = p->anim.speed;
            }
        }
    }

    p->hitbox.x = p->position.x + 4;
    p->hitbox.y = p->position.y + 4;
    p->hitbox.width = frame_width - 8;
    p->hitbox.height = frame_height - 8;

    int linha_atual = (int)(p->position.y / 32);
    int linha_minima = (int)(p->min_y_position / 32);
    if (linha_atual < linha_minima) {
        p->score += 10 * (linha_minima - linha_atual);
        p->min_y_position = linha_atual * 32;
    }
}

void draw_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row) {
    Rectangle src = animation_frame_rect(&p->anim, frame_width, frame_height, num_frames_per_row);
    Rectangle dest = { p->position.x + frame_width / 2.0f, p->position.y + frame_height / 2.0f, frame_width, frame_height };
    Vector2 origin = { frame_width / 2.0f, frame_height / 2.0f };
    DrawTexturePro(p->texture, src, dest, origin, p->rotation, WHITE);
}

void player_unload(struct player *p) {
    UnloadTexture(p->texture);
    UnloadTexture(p->death_texture);
}

void player_die(struct player *p, Sound *death_sound) {
    if (p->is_dead || p->game_over) return;

    p->is_dead = true;
    p->death_timer = 0.0f;
    p->death_animation.current_frame = p->death_animation.first_frame;
    p->death_animation.duration_left = p->death_animation.speed;
    PlaySound(*death_sound);

    p->lives--;
    if (p->lives <= 0) {
        p->game_over = true;
    }
}

void dead_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row) {
    Rectangle src = animation_frame_rect(&p->death_animation, frame_width, frame_height, num_frames_per_row);
    Rectangle dest = { p->position.x + frame_width / 2.0f, p->position.y + frame_height / 2.0f, frame_width, frame_height };
    Vector2 origin = { frame_width / 2.0f, frame_height / 2.0f };
    DrawTexturePro(p->death_texture, src, dest, origin, 0.0f, WHITE);
}

void get_home(struct player *p, Texture2D *sapo) {
    Vector2 home[5] = {
        { 22, 92 }, { 22 + (3 * 32), 92 }, { 22 + (6 * 32), 92 },
        { 22 + (9 * 32), 92 }, { 22 + (12 * 32), 92 }
    };

    for (int i = 0; i < 5; i++) {
        DrawRectangle(home[i].x, home[i].y, 20, 32, BLANK); // Tornando invisível, pois a lógica de evento deve cuidar disso
        if (p->oc_houses[i]) {
            DrawTexture(*sapo, home[i].x - 6 , home[i].y - 28, WHITE);
        }
    }

    for (int i = 0; i < 5; i++) {
        if (CheckCollisionRecs(p->hitbox, (Rectangle){ home[i].x, home[i].y, 20, 32 })) {
            if (IsKeyPressed(KEY_UP) && !p->oc_houses[i]) {
                p->score += 200;
                p->position = p->start_position;
                p->target_position = p->start_position;
                p->anim.current_frame = 0;
                p->rotation = 0.0f;
                p->is_dead = false;
                p->oc_houses[i] = true;
            }
        }
    }
}

bool player_on_trunk(struct player *p, Trunk *trunk_list, int trunk_count, float dt) {
    if (!p || !trunk_list || p->is_dead || p->game_over) return false;

    for (int i = 0; i < trunk_count; i++) {
        if (trunk_list[i].active && CheckCollisionRecs(p->hitbox, trunk_list[i].hitbox)) {
            if (!p->is_moving) {
                p->position.x += trunk_list[i].speed * dt;
                p->target_position.x = p->position.x;
            }
            return true;
        }
    }
    return false;
}
