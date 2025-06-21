#include "raylib.h"      // Biblioteca principal para gráficos, entrada e tipos
#include "raymath.h"     // Biblioteca para operações matemáticas com vetores
#include <stdlib.h>      // Para funções de alocação e controle de fluxo
#include <stdbool.h>     // Para o tipo bool

#include "player.h"      // Cabeçalho da TAD player
#include "animation.h"   // Cabeçalho da TAD de animação

bool oc_houses[5] = { false, false, false, false, false };

// Inicializa os dados do jogador, carregando texturas e definindo posições iniciais
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
    p->lives = 5;           // Número inicial de vidas
    p->score = 0;           // Pontuação inicial
    p->is_moving = false;   // O jogador começa parado
    p->rotation = 0.0f;     // Rotação inicial
    p->is_dead = false;     // Jogador começa vivo
    p->game_over = false;   // Jogo não acabou
    p->death_timer = 0.0f;  // Temporizador de morte zerado
    p->min_y_position = position.y; // Menor Y é a posição inicial

    // Inicializa o retângulo de colisão (hitbox)
    p->hitbox = (Rectangle){
        .x = position.x,
        .y = position.y,
        .width = p->texture.width / 5, // Supondo 5 frames por linha
        .height = p->texture.height / 1 // Supondo 1 linha
    };

    // Inicializa a animação do jogador
    p->anim = (Animation){
        .first_frame = 0,      // Primeiro frame da animação
        .last_frame = 4,       // Último frame da animação
        .current_frame = 0,    // Frame atual
        .duration_left = 0.1f, // Duração do frame atual
        .speed = 0.1f,         // Velocidade da animação
        .type = ONESHOT        // Tipo de animação (executa uma vez)
    };

    // Carrega a textura e animação de morte
    p->death_texture = LoadTexture("resources/sprites/morte.png");
    p->death_animation = (Animation) {
        .first_frame = 0,
        .last_frame = 2,
        .current_frame = 0,
        .duration_left = 0.1f,
        .speed = 0.1f,
        .type = ONESHOT
    };

    for (int i = 0; i < 5; i++) {
    p->oc_houses[i] = false;
    }

}

// Atualiza o estado do jogador (movimento, animação, colisão, etc)
void player_update(struct player *p, float dt, int frame_width, int frame_height, Sound *jump_sound) {
    // Se o jogador está morto, atualiza a animação de morte e temporizador
    if (p->is_dead) {
        animation_update(&p->death_animation, dt);
        p->death_timer += dt;

        // Após 2 segundos, revive o jogador ou finaliza o jogo
        if (!p->game_over && p->death_timer >= 2.0f) {
            p->is_dead = false;
            p->position = p->start_position;
            p->target_position = p->start_position;
            p->anim.current_frame = 0;
            p->rotation = 0.0f;
        }
        return; // Não processa o resto enquanto está morto
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

   

    // Só permite iniciar um novo movimento se o cooldown for zero e não estiver se movendo
    if (!p->is_moving && cooldown <= 0.0f) {
        Vector2 direction = {0.0f, 0.0f}; // Direção do movimento

        // Checa as teclas pressionadas e define a direção e rotação
        if (IsKeyPressed(KEY_RIGHT)) {
            direction.x = frame_width; // Move para a direita
            p->rotation = 90.0f;       // Rotação para a direita
            PlaySound(*jump_sound);    // Toca o som de pulo
        } else if (IsKeyPressed(KEY_LEFT)) {
            direction.x = -frame_width; // Move para a esquerda
            p->rotation = 270.0f;       // Rotação para a esquerda
            PlaySound(*jump_sound);
        } else if (IsKeyPressed(KEY_UP)) {
            direction.y = -frame_height; // Move para cima
            p->rotation = 0.0f;          // Rotação para cima
            PlaySound(*jump_sound);
        } else if (IsKeyPressed(KEY_DOWN)) {
            direction.y = frame_height; // Move para baixo
            p->rotation = 180.0f;       // Rotação para baixo
            PlaySound(*jump_sound);
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

    // Ganha pontos a cada 32 pixels percorridos negativamente no eixo Y
    int linha_atual = (int)(p->position.y / 32);
    int linha_minima = (int)(p->min_y_position / 32);
    if (linha_atual < linha_minima) {
        p->score += 10 * (linha_minima - linha_atual); // 10 pontos por linha pulada
        p->min_y_position = linha_atual * 32; // Atualiza para a nova linha mínima
    }
}

// Desenha o jogador na tela (quando está vivo)
void draw_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row) {
    // Define o retângulo da textura baseado na animação
    Rectangle src = animation_frame_rect(&p->anim, frame_width, frame_height, num_frames_per_row);

    // Define o retângulo de destino na tela (centralizado no sprite)
    Rectangle dest = {
        p->position.x + frame_width / 2.0f, // Centraliza horizontalmente
        p->position.y + frame_height / 2.0f, // Centraliza verticalmente
        frame_width,
        frame_height
    };

    // Define o ponto de origem como o centro do sprite
    Vector2 origin = (Vector2){ frame_width / 2.0f, frame_height / 2.0f };

    // Desenha o jogador com rotação
    DrawTexturePro(p->texture, src, dest, origin, p->rotation, WHITE);
}

// Libera os recursos do jogador (texturas, etc)
void player_unload(struct player *p) {
    UnloadTexture(p->texture);       // Libera a textura carregada do jogador
    UnloadTexture(p->death_texture); // Libera a textura de morte
}

// Executa a lógica de morte do jogador (animação, decremento de vidas, etc)
void player_die(struct player *p, Sound *death_sound) {
    if (p->is_dead || p->game_over) return; // Não faz nada se já está morto ou game over
    
    p->is_dead = true; // Marca como morto
    p->death_timer = 0.0f; // Reseta o temporizador de morte
    p->death_animation.current_frame = p->death_animation.first_frame; // Reseta a animação de morte
    p->death_animation.duration_left = p->death_animation.speed;
    PlaySound(*death_sound); // Toca o som de morte

    p->lives--; // Decrementa as vidas
    if (p->lives <= 0) {
        p->game_over = true; // Se acabou as vidas, marca game over
    }
}

// Desenha o jogador morto (animação de morte)
void dead_player(const struct player *p, int frame_width, int frame_height, int num_frames_per_row) {
    // Define o retângulo da textura baseado na animação de morte
    Rectangle src = animation_frame_rect(&p->death_animation, frame_width, frame_height, num_frames_per_row);

    // Define o retângulo de destino na tela (centralizado)
    Rectangle dest = {
        p->position.x + frame_width / 2.0f,
        p->position.y + frame_height / 2.0f,
        frame_width,
        frame_height
    };

    // Origem no centro do sprite
    Vector2 origin = { frame_width / 2.0f, frame_height / 2.0f };

    // Desenha a animação de morte
    DrawTexturePro(p->death_texture, src, dest, origin, 0.0f, WHITE);
}


void get_home(struct player *p, Texture2D *sapo) {
    Vector2 home[5] = {
        { 22, 92 },
        { 22 + (3 * 32), 92 },
        { 22 + (6 * 32), 92 },
        { 22 + (9 * 32), 92 },
        { 22 + (12 * 32), 92 }
    };

    for (int i = 0; i < 5; i++) {
        DrawRectangle(home[i].x, home[i].y, 20, 32, RED);
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
                p->oc_houses[i] = true; // Marca a casa como ocupada
            }
        }
    }
}