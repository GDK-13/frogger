#ifndef PLAYER_H
#define PLAYER_H

<<<<<<< HEAD
#include "raylib.h"      // Biblioteca principal para gráficos, entrada e tipos como Vector2, Texture2D, Rectangle, Sound
#include "animation.h"   // Inclui a estrutura e funções de animação
#include <stdbool.h>     // Para o tipo bool
=======
#include "raylib.h"
#include "animation.h"
#include "trunk.h"
#include <stdbool.h>
>>>>>>> 8651578 (Troncos)

// Estrutura para armazenar os dados do jogador
struct player {
    Vector2 position;        // Posição atual do jogador na tela
    Vector2 target_position; // Posição alvo para onde o jogador está se movendo
    Texture2D texture;       // Textura do sprite do jogador
    int score;               // Pontuação do jogador
    Animation anim;          // Estrutura de animação do jogador
    bool is_moving;          // Indica se o jogador está em movimento
    float rotation;          // Rotação do sprite do jogador (em graus)
    Rectangle hitbox;        // Retângulo de colisão do jogador
    bool is_dead;            // Indica se o jogador está morto
    bool game_over;          // Indica se o jogo acabou para o jogador
    int lives;               // Número de vidas restantes
    float death_timer;       // Temporizador para animação de morte
    Vector2 start_position;  // Posição inicial do jogador
    Texture2D death_texture; // Textura usada na animação de morte
    Animation death_animation; // Animação de morte do jogador
    float min_y_position;    // Menor posição Y alcançada (para pontuação)
    bool oc_houses[5];  // Indica quais casas já foram ocupadas
};

// Inicializa os dados do jogador
void player_init(struct player *p, const char *texture_path, Vector2 position);

// Atualiza o estado do jogador (movimento, animação, colisão, etc)
void player_update(struct player *p, float dt, int frame_width, int frame_height, Sound *jump_sound);

// Desenha o jogador na tela (quando está vivo)
void draw_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row);

// Libera os recursos do jogador (texturas, etc)
void player_unload(struct player *p);

// Executa a lógica de morte do jogador (animação, decremento de vidas, etc)
void player_die(struct player *p, Sound *death_sound);

// Desenha o jogador morto (animação de morte)
void dead_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row);

// Verifica se o jogador chegou ao seu destino e atualiza a posição
void get_home(struct player *p, Texture2D *sapo);
// Em caso de morte do jogador
void player_die(struct player *p);

// Desenhar animação de morte
void dead_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row);

// Verifica se o sapo está em cima de um objeto
bool player_on_trunk(struct player *p, Trunk *trunk, int log_count, float dt);

#endif
