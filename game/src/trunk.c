#include "raylib.h"       // Biblioteca principal para gráficos e entrada
#include "raymath.h"      // Biblioteca para operações matemáticas
#include <stdlib.h>       // Biblioteca padrão para alocação de memória e funções gerais
#include <stdbool.h>      // Biblioteca para manipulação de valores booleanos
#include <string.h> // Operações com strings

#include "trunk.h"      // Arquivo de cabeçalho para definição da TAD Trunk

#define MAX_TEXTURE_PARTS 4  // Número máximo de partes de uma textura 
#define MIN_TRUNK_DISTANCE 130 // Distância mínima entre troncos na mesma faixa

// Iniciliza os dados do tronco
static void trunk_init(struct Trunk *self, const char **texture_paths, Vector2 position, float speed, int parts) {
    // Número de partes não ultrapasse o máximo
    if (parts > MAX_TEXTURE_PARTS) {
        parts = MAX_TEXTURE_PARTS;
    }
    // Carrega as texturas 
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
    self->lane = (int)position.y; // Faixa (lane) definida pela coordenada Y 
    
    // Define a área de colisão do tronco com base na posição e quantidade de partes
    self->hitbox = (Rectangle) {
        .x = position.x,
        .y = position.y,
        .width = self->parts_width * parts,
        .height = self->textures[0].height-4
    };
}

// Atualiza o estado do tronco
static void trunk_update(struct Trunk *self, float dt) {
    if (!self->active) return;
    // Move o tronco na horizontal de acordo com a velocidade e tempo
    self->position.x += self->speed * dt;
    // Atualiza a posição do hitbox
    self->hitbox.x = self->position.x;
    self->hitbox.y = self->position.y;
}

// Desenha o tronco na tela
static void trunk_draw(const struct Trunk *self) {
    // Desenha cada parte do tronco na sequência
    for (int i = 0; i < self->parts; i++) {
        Vector2 dest = {
            self->position.x + i * self->parts_width,
            self->position.y
        };
    DrawTexture(self->textures[i], dest.x, dest.y, WHITE);
    }
}

// Libera os recursos 
static void trunk_unload(struct Trunk *self) {
    for (int i = 0; i < self->parts; i++) {
        if (self->textures[i].id != 0) {
        UnloadTexture(self->textures[i]);
        }
    }
}

// Verifica se há espaço livre em uma faixa para spawnar um novo tronco
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
            // Verifica colisão entre troncos
            if (CheckCollisionRecs(new_trunk, existing)) {
                return false; // Espaço ocupado
            }
        }
    }
    return true; // Espaço livre
}

// Cria um novo tronco na tela
void spawn_trunk(Trunk **trunk, int *trunk_count, const char **sprite_paths, float speed, int lane_index, int max_trunk_on_screen, const int *lane_y_positions, int parts) {
    // Número de troncos na tela não ultrapasse o máximo
    if (*trunk_count >= max_trunk_on_screen) {
        return;
    }
    // Define a posição inicial do tronco com base na direção (velocidade)
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

Trunk create_trunk() {
    Trunk trunk = {0};
    trunk.init = trunk_init;
    trunk.update = trunk_update;
    trunk.draw = trunk_draw;
    trunk.unload = trunk_unload;
    return trunk;
}
