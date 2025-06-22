#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "trunk.h"

#define MAX_TEXTURE_PARTS 4
#define MIN_TRUNK_DISTANCE 130

// ===================== INICIALIZAÇÃO DO TRONCO =====================
static void trunk_init(struct Trunk *self, const char **texture_paths, Vector2 position, float speed, int parts) {
    if (parts > MAX_TEXTURE_PARTS) parts = MAX_TEXTURE_PARTS;
    for (int i = 0; i < parts; i++) {
        self->textures[i] = LoadTexture(texture_paths[i]);
        if (self->textures[i].id == 0) {
            TraceLog(LOG_ERROR, "Erro ao carregar textura do tronco : %s", texture_paths[i]);
            exit(1);
        }
    }
    self->parts = parts;
    self->position = position;
    self->speed = speed;
    self->active = true;
    self->parts_width = (float)self->textures[0].width;
    self->lane = (int)position.y;
    self->hitbox = (Rectangle) {
        .x = position.x,
        .y = position.y,
        .width = self->parts_width * parts,
        .height = self->textures[0].height - 4
    };
}

// ===================== ATUALIZAÇÃO DO TRONCO =====================
static void trunk_update(struct Trunk *self, float dt) {
    if (!self->active) return;
    self->position.x += self->speed * dt;
    self->hitbox.x = self->position.x;
    self->hitbox.y = self->position.y;
}

// ===================== DESENHO DO TRONCO =====================
static void trunk_draw(const struct Trunk *self) {
    for (int i = 0; i < self->parts; i++) {
        Vector2 dest = {
            self->position.x + i * self->parts_width,
            self->position.y
        };
        DrawTexture(self->textures[i], dest.x, dest.y, WHITE);
    }
}

// ===================== LIBERAÇÃO DE RECURSOS =====================
static void trunk_unload(struct Trunk *self) {
    for (int i = 0; i < self->parts; i++) {
        if (self->textures[i].id != 0) {
            UnloadTexture(self->textures[i]);
        }
    }
}

// ===================== CHECA SE HÁ ESPAÇO NA FAIXA =====================
bool is_trunk_free(Trunk *trunk, int trunk_count, int lane_y, int new_x, int width) {
    for (int i = 0; i < trunk_count; i++) {
        if (trunk[i].position.y == lane_y) {
            Rectangle existing = trunk[i].hitbox;
            Rectangle new_trunk = {
                .x = new_x,
                .y = lane_y,
                .width = width,
                .height = existing.height
            };
            if (CheckCollisionRecs(new_trunk, existing)) {
                return false;
            }
        }
    }
    return true;
}

// ===================== SPAWN DE TRONCO =====================
void spawn_trunk(Trunk **trunk, int *trunk_count, const char **sprite_paths, float speed, int lane_index, int max_trunk_on_screen, const int *lane_y_positions, int parts) {
    if (*trunk_count >= max_trunk_on_screen) return;
    float x = (speed > 0) ? -((float)parts * 64) : GetScreenWidth() + 64;
    Vector2 position = { x, lane_y_positions[lane_index] };

    Trunk new_trunk = create_trunk();
    new_trunk.init(&new_trunk, sprite_paths, position, speed, parts);

    if (!is_trunk_free(*trunk, *trunk_count, new_trunk.lane, new_trunk.position.x, new_trunk.hitbox.width)) {
        new_trunk.unload(&new_trunk);
        return;
    }

    *trunk = realloc(*trunk, (*trunk_count + 1) * sizeof(Trunk));
    if (!*trunk) {
        TraceLog(LOG_ERROR, "Erro ao realocar troncos!");
        CloseWindow();
        exit(1);
    }

    (*trunk)[*trunk_count] = new_trunk;
    (*trunk_count)++;
}

// ===================== CRIAÇÃO DO TRONCO =====================
Trunk create_trunk() {
    Trunk trunk = {0};
    trunk.init = trunk_init;
    trunk.update = trunk_update;
    trunk.draw = trunk_draw;
    trunk.unload = trunk_unload;
    return trunk;
}
