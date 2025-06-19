#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdbool.h>

#include "enemies.h"

// Inicializa os dados do inimigo
void enemy_init(struct enemyCar *e, const char *texture_path, Vector2 position, float speed) {
    e->texture = LoadTexture(texture_path); 
    // Carrega a textura do inimigo (carro)
    if(e->texture.id == 0) {
        TraceLog(LOG_ERROR, "Erro ao carregar textura do inimigo : %s", texture_path);
        exit(1);
    }
    // Define a posição, velocidade e marca o carro como ativo
    e->position = position;
    e->speed = speed;
    e->active = true;
    
    // Cria a área de colisão
    e->hitbox = (Rectangle) {
        .x = position.x,
        .y = position.y,
        .width = e->texture.width,
        .height = e->texture.height
    };
}

//Atualiza o estado do inimigo
void enemy_update(struct enemyCar *e, float dt) {
    // Movimento do carro
    e->position.x += e->speed * dt;

    // Quando o carro sai de um lado da tela, ele reaparece do lado oposto: se for para a direita, surge na esquerda; se for para a esquerda, surge na direita
    if(e->speed > 0 && e->position.x > GetScreenWidth()) {
        e->position.x = -e->texture.width;
    } else if (e->speed < 0 && e->position.x < -e->texture.width) {
        e->position.x = GetScreenWidth();
    }

    // Atualizaa a área de colisão
    e->hitbox.x = e->position.x;
    e->hitbox.y = e->position.y;
}

//Desenha o inimigo na tela
void enemy_draw(const struct enemyCar *e) {
    DrawTexture(e->texture, e->position.x, e->position.y, WHITE);
}

//Libera os recursos do inimigo
void enemy_unload(struct enemyCar *e) {
    UnloadTexture(e->texture);
}

// Verifica colisões com o jogador
bool enemy_check_collision(const struct enemyCar *e, Rectangle player_hitbox) {
    return CheckCollisionRecs(e->hitbox, player_hitbox);
}

