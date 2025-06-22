#include "hud.h"
#include "events.h"
#include "raylib.h"
#include <stdio.h>

// ===================== HUD PRINCIPAL =====================
void draw_hud(Font font, int vidas, int score, Texture2D life, Time *timer) {
    Color light = (Color){ 205, 178, 171, 255 };
    Color dark  = (Color){ 46, 22, 51, 255 };

    // --- Ícones de vida ---
    typedef struct life_icon {
        Texture2D texture;
        Vector2 position;
    } LifeIcon;

    LifeIcon icon[5];
    for (int i = 0; i < 5; i++) icon[i].texture = life;
    for (int i = 0; i < vidas; i++) {
        icon[i].position = (Vector2){ i * 32, 480 };
        DrawTexture(icon[i].texture, icon[i].position.x, icon[i].position.y, WHITE);
    }

    // --- Score centralizado ---
    float font_size = 24;
    int screen_width = GetScreenWidth();
    const char *score_label = "SCORE";
    char score_number[32];
    sprintf(score_number, "%d", score);

    Vector2 label_size  = MeasureTextEx(font, score_label, font_size, 0);
    Vector2 number_size = MeasureTextEx(font, score_number, font_size, 0);
    Vector2 label_pos  = { (screen_width - label_size.x+2) / 2, 1 };
    Vector2 number_pos = { (screen_width - number_size.x+2) / 2, 34 };

    DrawTextEx(font, score_label, label_pos, font_size, 0, light);
    DrawRectangle(179, 30, label_size.x, 32, light);
    DrawTextEx(font, score_number, number_pos, font_size, 0, dark);

    // --- Instruções do HUD ---
    DrawTextEx(font, "ESC", (Vector2){ 2, 1 }, font_size, 0, light);
    DrawTextEx(font, "SAIR", (Vector2){ 2, 32 }, font_size, 0, light);

    Vector2 enter_size = MeasureTextEx(font, "ENTER", font_size, 0);
    Vector2 pausar_size = MeasureTextEx(font, "PAUSE", font_size, 0);
    Vector2 enter_pos = { screen_width - enter_size.x, 1 };
    Vector2 pausar_pos = { screen_width - pausar_size.x, 32 };

    DrawTextEx(font, "ENTER", enter_pos, font_size, 0, light);
    DrawTextEx(font, "PAUSE", pausar_pos, font_size, 0, light);

    // --- Cronômetro ---
    char timer_text[16];
    sprintf(timer_text, "%02d %02d", timer->minutes, timer->seconds);
    Vector2 timer_size = MeasureTextEx(font, timer_text, 20, 0);
    Vector2 timer_position = { GetScreenWidth() - timer_size.x - 13, 484 };

    DrawRectangle(timer_position.x - 4, timer_position.y - 2, timer_size.x + 20, 28, light);
    DrawRectangle(timer_position.x + 36, timer_position.y + 16, 4, 4, dark);
    DrawRectangle(timer_position.x + 36, timer_position.y + 5, 4, 4, dark);

    DrawTextEx(font, timer_text, timer_position, font_size, 0, dark);
    DrawTextEx(font, "TIMER", (Vector2){ timer_position.x - 90, timer_position.y }, font_size, 0, light);
}
