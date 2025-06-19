#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdbool.h>

#include "player.h"
#include "animation.h"

// Inicializa os dados do jogador
void player_init(struct player *p, const char *texture_path, Vector2 position) {
    // Define a posição inicial do jogador
    p->start_position = position;
    p->target_position = position;
    p->position = position;

    // Carrega a textura do jogador
    p->texture = LoadTexture(texture_path);
    if (p->texture.id == 0) {
        TraceLog(LOG_ERROR, "Erro ao carregar sprite do jogador: %s", texture_path);
        exit(1);
    }

    // Inicializa os atributos do jogador
    p->lives = 3;
    p->score = 0; // Pontuação inicial
    p->hitbox = (Rectangle){
        .x = position.x,
        .y = position.y,
        .width = p->texture.width / 5, // Supondo que o sprite tem 5 frames por linha
        .height = p->texture.height / 1 // Supondo que o sprite tem 1 linha
    };
    p->is_moving = false; // O jogador começa parado
    p->rotation = 0.0f; // Rotação inicial

    // Inicializa a animação do jogador
    p->anim = (Animation){
        .first_frame = 0, // Primeiro frame da animação
        .last_frame = 4,  // Último frame da animação
        .current_frame = 0, // Frame atual
        .duration_left = 0.1f, // Duração do frame atual
        .speed = 0.1f, // Velocidade da animação
        .type = ONESHOT // Tipo de animação (executa uma vez)
    };

    p->death_texture = LoadTexture("resources/sprites/morte.png");
    p->death_animation = (Animation) {
        .first_frame = 0, // Primeiro frame da animação
        .last_frame = 2,  // Último frame da animação
        .current_frame = 0, // Frame atual
        .duration_left = 0.1f, // Duração do frame atual
        .speed = 0.1f, // Velocidade da animação
        .type = ONESHOT // Tipo de animação (executa uma vez)
    };
}

// Atualiza o estado do jogador
void player_update(struct player *p, float dt, int frame_width, int frame_height) {
    if (p->is_dead) {
        animation_update(&p->death_animation, dt);
        p->death_timer += dt;
    
    if (!p->game_over && p->death_timer >= 2.0f) {
            p->is_dead = false;
            p->position = p->start_position;
            p->target_position = p->start_position;
            p->anim.current_frame = 0;
            p->rotation = 0.0f;
            }
    return;
    }

    static float cooldown = 0.0f; // Tempo de espera entre movimentos

    // Atualiza o cooldown (tempo de espera para o próximo movimento)
    if (cooldown > 0.0f) {
        cooldown -= dt;
    }

    // Se o jogador está se movendo, interpola a posição atual até a posição alvo
    if (p->is_moving) {
        p->position = Vector2Lerp(p->position, p->target_position, dt * 17.0f); // Suaviza o movimento
        animation_update(&p->anim, dt); // Atualiza a animação

        // Verifica se o jogador chegou na posição alvo
        if (Vector2Distance(p->position, p->target_position) < 0.1f) {
            p->position = p->target_position; // Ajusta a posição final
            p->is_moving = false; // Para o movimento
            p->anim.current_frame = p->anim.first_frame; // Reseta a animação
            p->anim.duration_left = p->anim.speed; // Reseta o tempo do frame

            // Reinicia o cooldown após o movimento
            cooldown = 0.01f; // Define o tempo de espera
        }
    }

    if (p->position.y < p->score) {
        p->score = p->position.y;
    }

    // Só permite iniciar um novo movimento se o cooldown for zero
    if (!p->is_moving && cooldown <= 0.0f) {
        Vector2 direction = {0.0f, 0.0f}; // Direção do movimento
        if (IsKeyPressed(KEY_RIGHT)) {
            direction.x = frame_width; // Move para a direita
            p->rotation = 90.0f; // Rotação para a direita
        } else if (IsKeyPressed(KEY_LEFT)) {
            direction.x = -frame_width; // Move para a esquerda
            p->rotation = 270.0f; // Rotação para a esquerda
        } else if (IsKeyPressed(KEY_UP)) {
            direction.y = -frame_height; // Move para cima
            p->rotation = 0.0f; // Rotação para cima
        } else if (IsKeyPressed(KEY_DOWN)) {
            direction.y = frame_height; // Move para baixo
            p->rotation = 180.0f; // Rotação para baixo
        }

        // Se uma direção foi definida, calcula a nova posição alvo
        if (direction.x != 0.0f || direction.y != 0.0f) {
            Vector2 new_target_position = Vector2Add(p->position, direction);

            // Limita o movimento para não sair da tela ou ultrapassar a altura mínima de 96
            if (new_target_position.x >= 0 && 
                new_target_position.x + frame_width <= GetScreenWidth() &&
                new_target_position.y >= 96 &&
                new_target_position.y + frame_height <= GetScreenHeight()) {
                p->target_position = new_target_position; // Define a nova posição alvo
                p->is_moving = true; // Inicia o movimento
                p->anim.current_frame = p->anim.first_frame; // Reseta a animação
                p->anim.duration_left = p->anim.speed; // Reseta o tempo do frame
            }
        }
    }

    // Atualiza a hitbox para acompanhar o jogador
    p->hitbox.x = p->position.x + 4;
    p->hitbox.y = p->position.y + 4;
    p->hitbox.width = frame_width - 8;
    p->hitbox.height = frame_height - 8;
}

// Desenha o jogador na tela
void draw_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row) {
    // Define o retângulo da textura baseado na animação
    Rectangle src = animation_frame_rect(&p->anim, frame_width, frame_height, num_frames_per_row);

    // Define o retângulo de destino na tela
    Rectangle dest = {
        p->position.x + frame_width / 2.0f, // Ajusta para alinhar o canto superior esquerdo
        p->position.y + frame_height / 2.0f, // Ajusta para alinhar o canto superior esquerdo
        frame_width,
        frame_height
    };

    // Define o ponto de origem como o centro do sprite
    Vector2 origin = (Vector2){ frame_width / 2.0f, frame_height / 2.0f };

    // Desenha o jogador com rotação
    DrawTexturePro(p->texture, src, dest, origin, p->rotation, WHITE);
}

// Libera os recursos do jogador
void player_unload(struct player *p) {
    UnloadTexture(p->texture); // Libera a textura carregada
    UnloadTexture(p->death_texture);
}

void player_die(struct player *p) {
    if (p->is_dead || p->game_over) return;
    
    p->is_dead = true;
    p->death_timer = 0.0f;
    p->death_animation.current_frame = p->death_animation.first_frame;
    p->death_animation.duration_left = p->death_animation.speed;

    p->lives--;
    if (p->lives <= 0) {
        p->game_over = true;
    }
}

void dead_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row) {
    Rectangle src = animation_frame_rect(&p->death_animation, frame_width, frame_height, num_frames_per_row);

    Rectangle dest = {
        p->position.x + frame_width / 2.0f,
        p->position.y + frame_height / 2.0f,
        frame_width,
        frame_height
    };

    Vector2 origin = { frame_width / 2.0f, frame_height / 2.0f };

    DrawTexturePro(p->death_texture, src, dest, origin, 0.0f, WHITE);
}