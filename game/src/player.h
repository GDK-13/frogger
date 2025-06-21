#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "animation.h"
#include "trunk.h"
#include "turtle.h"
#include <stdbool.h>

// Estrutura para armazenar os dados do jogador
struct player {
    Vector2 position;
    Vector2 target_position;
    Texture2D texture;
    int score;
    Animation anim;
    bool is_moving;
    float rotation;
    Rectangle hitbox;
    bool is_dead;
    bool game_over;
    int lives;
    float death_timer;
    Vector2 start_position;
    Texture2D death_texture;
    Animation death_animation;
    float min_y_position;
    bool oc_houses[5];
};

// Inicializa os dados do jogador
void player_init(struct player *p, const char *texture_path, Vector2 position);

// Atualiza o estado do jogador
void player_update(struct player *p, float dt, int frame_width, int frame_height, Sound *jump_sound);

// Desenha o jogador na tela
void draw_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row);

// Libera os recursos do jogador
void player_unload(struct player *p);

// Executa a lógica de morte do jogador
void player_die(struct player *p, Sound *death_sound);

// Desenha o jogador morto
void dead_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row);

// Verifica se o jogador chegou em uma das "casas"
void get_home(struct player *p, Texture2D *sapo);

// Verifica se o jogador está em cima de um tronco
bool player_on_trunk(struct player *p, Trunk *trunk, int trunk_count, float dt);

// Verifica se o jogador está em cima de uma tartaruga
bool player_on_turtle(struct player *player, Turtle *turtle, int turtle_count, float dt);

#endif
