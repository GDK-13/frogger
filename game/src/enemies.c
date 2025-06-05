#include "raylib.h"
#include "enemies.h"



void UpdateEnemies(Enemy enemies[], bool left_right, int i){
    if(left_right) {
            if ((enemies[i].position.x <= 448)) {
                enemies[i].position.x += enemies[i].speed;
            } else {
                enemies[i].position.x = -64;
            }
    } else {
            if ((enemies[i].position.x >= -32)) {
                enemies[i].position.x -= enemies[i].speed;
            } else {
                enemies[i].position.x = 448;
            }
    }
}


void DrawEnemies(Enemy enemies[]) {
    for (int i = 0; i < 5; i++) {
        DrawTexture(enemies[i].texture, enemies[i].position.x, enemies[i].position.y, WHITE);
    }
}
