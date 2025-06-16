#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdbool.h>

#include "player.h"
#include "animation.h"

void player_init(struct player *p, const char *texture_path, Vector2 position) {
    p->position = position;
    p->target_position = position;
    p->texture = LoadTexture(texture_path);

    if (p->texture.id == 0) {
        TraceLog(LOG_ERROR, "Erro ao carregar sprite do jogador: %s", texture_path);
        exit(1);
    }

    p->score = 0;
    p->hitbox = (Rectangle){
        .x = position.x,
        .y = position.y,
        .width = p->texture.width / 5, // Supondo que o sprite tem 4 frames por linha
        .height = p->texture.height / 1 // Supondo que o sprite tem 1 linha
    };
    p->is_moving = false;
    p->rotation = 0.0f;

    p->anim = (Animation){
        .first_frame = 0,
        .last_frame = 4,
        .current_frame = 0,
        .duration_left = 0.1f,
        .speed = 0.1f,
        .type = ONESHOT
    };
}

void player_update(struct player *p, float dt, int frame_width, int frame_height) {
    static float cooldown = 0.0f; // Tempo de espera entre movimentos

    // Atualiza o cooldown
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

            // Reinicia o cooldown após o movimento
            cooldown = 0.01f; // Define o tempo de espera (0.2 segundos)
        }
    }

    // Só permite iniciar um novo movimento se o cooldown for zero
    if (!p->is_moving && cooldown <= 0.0f) {
        Vector2 direction = {0.0f, 0.0f};
        if (IsKeyPressed(KEY_RIGHT)) {
            direction.x = frame_width;
            p->rotation = 90.0f; // Direita
        } else if (IsKeyPressed(KEY_LEFT)) {
            direction.x = -frame_width;
            p->rotation = 270.0f; // Esquerda
        } else if (IsKeyPressed(KEY_UP)) {
            direction.y = -frame_height;
            p->rotation = 0.0f; // Cima
        } else if (IsKeyPressed(KEY_DOWN)) {
            direction.y = frame_height;
            p->rotation = 180.0f; // Baixo
        }

        if (direction.x != 0.0f || direction.y != 0.0f) {
            Vector2 new_target_position = Vector2Add(p->position, direction);

            // Limita o movimento para não sair da tela ou ultrapassar a altura 96
            if (new_target_position.x >= 0 && 
                new_target_position.x + frame_width <= GetScreenWidth() &&
                new_target_position.y >= 96 &&
                new_target_position.y + frame_height <= GetScreenHeight()) {
                p->target_position = new_target_position;
                p->is_moving = true;
                p->anim.current_frame = p->anim.first_frame;
                p->anim.duration_left = p->anim.speed;
            }
        }
    }

    // Atualiza a hitbox para acompanhar o jogador SEMPRE
    p->hitbox.x = p->position.x;
    p->hitbox.y = p->position.y;
    p->hitbox.width = frame_width;
    p->hitbox.height = frame_height;
}

void draw_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row) {
    Rectangle src = animation_frame_rect(&p->anim, frame_width, frame_height, num_frames_per_row);
    Rectangle dest = {
        p->position.x + frame_width / 2.0f, // Ajusta para alinhar o canto superior esquerdo
        p->position.y + frame_height / 2.0f, // Ajusta para alinhar o canto superior esquerdo
        frame_width,
        frame_height
    };

    // Define o ponto de origem como o centro do sprite
    Vector2 origin = (Vector2){ frame_width / 2.0f, frame_height / 2.0f };

    DrawTexturePro(p->texture, src, dest, origin, p->rotation, WHITE);
}

void player_unload(struct player *p) {
    UnloadTexture(p->texture);
}
