#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "turtle.h"

#define MIN_TURTLE_DISTANCE 100

// ===================== INICIALIZAÇÃO DA TARTARUGA =====================
static void turtle_init(struct Turtle *self, const char *turtle, Vector2 position, float speed, int parts) {
    self->texture = LoadTexture(turtle);
    if (self->texture.id == 0) {
        TraceLog(LOG_ERROR, "Erro ao carregar textura do inimigo : %s", turtle);
        exit(1);
    }
    self->position = position;
    self->speed = speed;
    self->active = true;
    self->lane = (int)position.y;
    self->parts = parts;
    self->hitbox = (Rectangle) {
        .x = self->position.x,
        .y = self->position.y,
        .width = self->texture.width * parts,
        .height = self->texture.height - 4
    };
}

// ===================== ATUALIZAÇÃO DA TARTARUGA =====================
static void turtle_update(struct Turtle *self, float dt) {
    if (!self->active) return;
    self->position.x += self->speed * dt;
    self->hitbox.x = self->position.x;
    self->hitbox.y = self->position.y;
}

// ===================== DESENHO DA TARTARUGA =====================
static void turtle_draw(const struct Turtle *self) {
    if (!self || self->parts <= 0) return;
    for (int i = 0; i < self->parts; i++) {
        float segment_x = self->position.x + i * self->texture.width;
        float segment_y = self->position.y;
        Vector2 origin = { self->texture.width / 2.0f, self->texture.height / 2.0f };
        Vector2 draw_pos = { segment_x + origin.x + 16, segment_y + origin.y + 16 };

        if (self->position.y > 200) {
            DrawTextureEx(self->texture, draw_pos, 180.0f, 1.0f, WHITE);
        } else {
            DrawTexture(self->texture, segment_x, segment_y, WHITE);
        }
    }
}

// ===================== LIBERAÇÃO DE RECURSOS =====================
static void turtle_unload(struct Turtle *self) {
    UnloadTexture(self->texture);
}

// ===================== CHECA SE HÁ ESPAÇO NA FAIXA =====================
bool is_turtle_free(Turtle *turtle, int turtle_count, int lane_y, int new_x, int width) {
    for (int i = 0; i < turtle_count; i++) {
        if (turtle[i].position.y == lane_y) {
            Rectangle existing = turtle[i].hitbox;
            Rectangle new_turtle = {
                .x = new_x,
                .y = lane_y,
                .width = width,
                .height = existing.height
            };
            if (CheckCollisionRecs(new_turtle, existing) || fabs(existing.x - new_x) < MIN_TURTLE_DISTANCE) {
                return false;
            }
        }
    }
    return true;
}

// ===================== SPAWN DE TARTARUGA =====================
void spawn_turtle(Turtle **turtle, int *turtle_count, const char *sprite_path, float speed, int lane_index, int max_turtle_on_screen, const int *lane_y_positions, int parts) {
    if (*turtle_count >= max_turtle_on_screen) return;

    float x = (speed > 0) ? -((float)parts * 64) : GetScreenWidth() + 64;
    Vector2 position = { x, lane_y_positions[lane_index] };

    Turtle new_turtle = create_turtle();
    new_turtle.init(&new_turtle, sprite_path, position, speed, parts);

    if (!is_turtle_free(*turtle, *turtle_count, new_turtle.lane, new_turtle.position.x, new_turtle.hitbox.width)) {
        new_turtle.unload(&new_turtle);
        return;
    }

    Turtle *temp = realloc(*turtle, (*turtle_count + 1) * sizeof(Turtle));
    if (!temp) {
        TraceLog(LOG_ERROR, "Erro ao realocar tartarugas!");
        CloseWindow();
        exit(1);
    }

    *turtle = temp;
    (*turtle)[*turtle_count] = new_turtle;
    (*turtle_count)++;
}

// ===================== CRIAÇÃO DA TARTARUGA =====================
Turtle create_turtle() {
    Turtle turtle;
    turtle.init = turtle_init;
    turtle.update = turtle_update;
    turtle.draw = turtle_draw;
    turtle.unload = turtle_unload;
    return turtle;
}