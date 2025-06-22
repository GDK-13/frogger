#include "raylib.h"       // Biblioteca principal para gráficos e entrada
#include "raymath.h"      // Biblioteca para operações matemáticas
#include <stdlib.h>       // Biblioteca padrão para alocação de memória e funções gerais
#include <stdbool.h>      // Biblioteca para manipulação de valores booleanos
#include <string.h> // Operações com strings

#include "turtle.h"      // Arquivo de cabeçalho para definição da TAD Turtle
 
#define MIN_TURTLE_DISTANCE 100 // Distância mínima na mesma faixa

static void turtle_init(struct Turtle *self, const char *texture_path, Vector2 position, float speed, int parts) {
    // Carrega a textura da tartaruga
    self->texture = LoadTexture(texture_path);
    if (self->texture.id == 0) { // Verifica se a textura foi carregada corretamente
        TraceLog(LOG_ERROR, "Erro ao carregar textura do inimigo : %s", texture_path);
        exit(1); // Sai do programa com erro
    }

    // Define os atributos iniciais da tartaruga
    self->position = position; // Posição inicial
    self->speed = speed;       // Velocidade inicial
    self->active = true;       // Marca a tartaruga como ativo
    self->lane = (int)position.y; // Faixa (lane) definida pela coordenada Y 
    self->parts = parts;

    // Define o hitbox da tartaruga com base na posição e tamanho da textura
    self->hitbox = (Rectangle) {
        .x = self->position.x,
        .y = self->position.y,
        .width = self->texture.width * parts,
        .height = self->texture.height-4
    };
}

// Atualiza o estado da tartaruga
static void turtle_update(struct Turtle *self, float dt) {
    if (!self->active) return;
    // Move o tronco na horizontal de acordo com a velocidade e tempo
    self->position.x += self->speed * dt;
    // Atualiza a posição do hitbox
    self->hitbox.x = self->position.x;
    self->hitbox.y = self->position.y;
}

// Função para desenhar a tartaruga na tela
static void turtle_draw(const struct Turtle *self) {
    if (!self || self->parts <= 0) return;

    for (int i = 0; i < self->parts; i++) {
        // Calcula a posição bruta de cada segmento da tartaruga
        float segment_x = self->position.x + i * self->texture.width;
        float segment_y = self->position.y;

        // Origem no centro do sprite
        Vector2 origin = {
            self->texture.width / 2.0f,
            self->texture.height / 2.0f
        };

        // Posição corrigida para desenhar considerando a origem central
        Vector2 draw_pos = {
            segment_x + origin.x+16,
            segment_y + origin.y+16
        };

        if (self->position.y > 200) {
            DrawTextureEx(
                self->texture,
                draw_pos,         // posição corrigida
                180.0f,           // rotação
                1.0f,             // escala
                WHITE
            );
        } else {
            // Desenho padrão sem rotação
            DrawTexture(
                self->texture,
                segment_x,
                segment_y,
                WHITE
            );
        }
    }
}


// Função para liberar os recursos da tartaruga
static void turtle_unload(struct Turtle *self) {
    // Libera a textura carregada
    UnloadTexture(self->texture);
}

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
            // Verifica colisão entre tartarugas
            if (CheckCollisionRecs(new_turtle, existing) || fabs(existing.x - new_x) < MIN_TURTLE_DISTANCE ) {
                return false; // Espaço ocupado
            }
        }
    }
    return true; // Espaço livre
}

void spawn_turtle(Turtle **turtle, int *turtle_count, const char *sprite_path, float speed, int lane_index, int max_turtle_on_screen, const int *lane_y_positions, int parts) {
    if (*turtle_count >= max_turtle_on_screen) {
        return;
    }

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

Turtle create_turtle() {
    Turtle turtle;
    turtle.init = turtle_init;
    turtle.update = turtle_update;
    turtle.draw = turtle_draw;
    turtle.unload = turtle_unload;
    return turtle;
}