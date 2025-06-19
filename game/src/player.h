#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "animation.h"
#include <stdbool.h>

// Estrutura para armazenar os dados do jogador
struct player {
    Vector2 position;        // Posição atual do jogador
    Vector2 target_position; // Posição alvo para o movimento
    Texture2D texture;       // Textura do jogador
    int score;               // Pontuação do jogador
    Animation anim;          // Animação do jogador
    bool is_moving;          // Indica se o jogador está se movendo
    float rotation;          // Rotação do jogador
    Rectangle hitbox;        // Hitbox do jogador para colisões
    bool is_dead;
    bool game_over;
    int lives;
    float death_timer;
    Vector2 start_position;
    Texture2D death_texture;
    Animation death_animation;
    float min_y_position;
};

// Inicializa os dados do jogador
void player_init(struct player *p, const char *texture_path, Vector2 position);

// Atualiza o estado do jogador
void player_update(struct player *p, float dt, int frame_width, int frame_height, Sound *jump_sound);

// Desenha o jogador na tela
void draw_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row);

// Libera os recursos do jogador
void player_unload(struct player *p);

void player_die(struct player *p, Sound *death_sound);

void dead_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row);

#endif
