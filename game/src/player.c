#include "raylib.h"
#include "player.h"

Vector2 playerPosition = { 256.0f, 480.0f };

static int playerSpeed = 32;
static int key = 0;
static float waitTime = 0.2f; // Tempo entre movimentos em segundos
static double lastMoveTime = 0; // Tempo do último movimento

void UpdatePlayer(void) {
    double currentTime = GetTime();

    int spriteWidth = 32;  // ajuste para o tamanho real do seu sprite, se necessário
    int spriteHeight = 32;

    if ((currentTime - lastMoveTime) >= waitTime) {
        if (IsKeyDown(KEY_RIGHT)) {
            if (playerPosition.x + playerSpeed <= 448 - spriteWidth) {
                playerPosition.x += playerSpeed;
                key = 1;
                lastMoveTime = currentTime;
            }
        }
        else if (IsKeyDown(KEY_LEFT)) {
            if (playerPosition.x - playerSpeed >= 0) {
                playerPosition.x -= playerSpeed;
                key = 2;
                lastMoveTime = currentTime;
            }
        }
        else if (IsKeyDown(KEY_UP)) {
            if (playerPosition.y - playerSpeed >= 0) {
                playerPosition.y -= playerSpeed;
                key = 3;
                lastMoveTime = currentTime;
            }
        }
        else if (IsKeyDown(KEY_DOWN)) {
            if (playerPosition.y + playerSpeed <= 512 - spriteHeight) {
                playerPosition.y += playerSpeed;
                key = 4;
                lastMoveTime = currentTime;
            }
        }
    }
}

void DrawPlayer(Texture2D player) {
    Rectangle source = { 0.0f, 0.0f, (float)player.width, (float)player.height };
    Vector2 origin = { player.width / 2.0f, player.height / 2.0f };
    Rectangle dest = { playerPosition.x + origin.x, playerPosition.y + origin.y, player.width, player.height };

    float rotation = 0.0f;

    switch (key) {
        case 1: rotation = 90.0f; break;    // Direita
        case 2: rotation = -90.0f; break;   // Esquerda
        case 3: rotation = 0.0f; break;     // Cima
        case 4: rotation = 180.0f; break;   // Baixo
    }

    DrawTexturePro(player, source, dest, origin, rotation, WHITE);
}
