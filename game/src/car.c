#include "raylib.h"       // Biblioteca principal para gráficos e entrada
#include "raymath.h"      // Biblioteca para operações matemáticas
#include <stdlib.h>       // Biblioteca padrão para alocação de memória e funções gerais
#include <stdbool.h>      // Biblioteca para manipulação de valores booleanos

#include "car.h"      // Arquivo de cabeçalho para definição da TAD EnemyCar

// Função para inicializar os dados do inimigo
static void enemy_init(struct EnemyCar *self, const char *texture_path, Vector2 position, float speed) {
    // Carrega a textura do inimigo a partir do caminho fornecido
    self->texture = LoadTexture(texture_path);
    if (self->texture.id == 0) { // Verifica se a textura foi carregada corretamente
        TraceLog(LOG_ERROR, "Erro ao carregar textura do inimigo : %s", texture_path);
        exit(1); // Sai do programa com erro
    }

    // Define os atributos iniciais do inimigo
    self->position = position; // Posição inicial
    self->speed = speed;       // Velocidade inicial
    self->active = true;       // Marca o inimigo como ativo

    // Define o hitbox do inimigo com base na posição e tamanho da textura
    self->hitbox = (Rectangle) {
        .x = position.x,
        .y = position.y,
        .width = self->texture.width,
        .height = self->texture.height
    };
}

// Função para atualizar o estado do inimigo
static void enemy_update(struct EnemyCar *self, float dt) {
    // Atualiza a posição do inimigo com base na velocidade e no tempo decorrido
    self->position.x += self->speed * dt;

    // Verifica se o inimigo saiu da tela e reposiciona-o no lado oposto
    if (self->speed > 0 && self->position.x > GetScreenWidth()) {
        self->position.x = -self->texture.width; // Reposiciona no lado esquerdo
    } else if (self->speed < 0 && self->position.x < -self->texture.width) {
        self->position.x = GetScreenWidth(); // Reposiciona no lado direito
    }

    // Atualiza o hitbox com a nova posição
    self->hitbox.x = self->position.x;
    self->hitbox.y = self->position.y;
}

// Função para desenhar o inimigo na tela
static void enemy_draw(const struct EnemyCar *self) {
    // Desenha a textura do inimigo na posição atual
    DrawTexture(self->texture, self->position.x, self->position.y, WHITE);
}

// Função para liberar os recursos do inimigo
static void enemy_unload(struct EnemyCar *self) {
    // Libera a textura carregada
    UnloadTexture(self->texture);
}

// Função para verificar colisões com o jogador
static bool enemy_check_collision(const struct EnemyCar *self, Rectangle player_hitbox) {
    // Retorna verdadeiro se o hitbox do inimigo colidir com o hitbox do jogador
    return CheckCollisionRecs(self->hitbox, player_hitbox);
}

// Função para verificar se há colisão entre um carro novo e os carros ativos na mesma faixa
bool is_lane_free(EnemyCar *active_cars, int active_car_count, int lane_index, float x_position, float car_width) {
    for (int i = 0; i < active_car_count; i++) {
        // Verifica se o carro ativo está na mesma faixa
        if (active_cars[i].position.y == lane_index) {
            // Verifica se a posição do carro novo está muito próxima de um carro ativo
            if (fabs(active_cars[i].position.x - x_position) < car_width) {
                return false; // Faixa ocupada
            }
        }
    }
    return true; // Faixa livre
}

// Função para spawnar um novo carro
void spawn_car(EnemyCar **active_cars, int *active_car_count, const char *sprite_path, float speed, int lane_index, int max_cars_on_screen, const int *lane_y_positions) {
    if (*active_car_count >= max_cars_on_screen) {
        return; // Não permite spawnar mais carros se o limite for atingido
    }

    // Define a posição inicial do carro com base na direção da velocidade
    float x = (speed > 0) ? -64 : GetScreenWidth() + 64;

    // Verifica se a faixa está livre para spawnar o carro
    if (!is_lane_free(*active_cars, *active_car_count, lane_y_positions[lane_index], x, 64)) {
        return; // Não permite spawnar se a faixa estiver ocupada
    }

    // Realoca memória para adicionar um novo carro ao array dinâmico
    *active_cars = realloc(*active_cars, (*active_car_count + 1) * sizeof(EnemyCar));
    if (!*active_cars) { // Verifica se a realocação foi bem-sucedida
        TraceLog(LOG_ERROR, "Erro ao alocar memória para os carros!");
        CloseWindow(); // Fecha a janela
        exit(1); // Sai do programa com erro
    }

    // Cria e inicializa o novo carro
    (*active_cars)[*active_car_count] = create_enemy_car();
    (*active_cars)[*active_car_count].init(&(*active_cars)[*active_car_count], sprite_path, (Vector2){x, lane_y_positions[lane_index]}, speed);
    (*active_car_count)++; // Incrementa o contador de carros ativos
}

// Função para criar um novo inimigo
EnemyCar create_enemy_car() {
    // Retorna uma instância de EnemyCar com os métodos associados
    EnemyCar enemy = {
        .init = enemy_init,
        .update = enemy_update,
        .draw = enemy_draw,
        .unload = enemy_unload,
        .check_collision = enemy_check_collision
    };
    return enemy;
}
