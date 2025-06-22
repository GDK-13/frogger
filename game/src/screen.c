#include "screen.h"
#include <stdio.h>

// ===================== CORES =====================
Color light = (Color){ 205, 178, 171, 255 };
Color dark  = (Color){ 46, 22, 51, 255 };

// ===================== BOTÃO =====================
// Verifica se o botão foi clicado
static bool ButtonClicked(Rectangle rect) {
    return CheckCollisionPointRec(GetMousePosition(), rect) &&
           IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// ===================== INICIALIZAÇÃO DAS TELAS =====================
static void screen_init(ScreenManager *self) {
    int centerX = GetScreenWidth() / 2;

    self->font = LoadFontEx("resources/fontes/8_bit_fortress/8-bit-fortress.ttf", 32, NULL, 0);
    if (self->font.baseSize == 0) {
        printf("Erro: Não foi possível carregar a fonte 8-bit\n");
        CloseWindow();
    }

    self->backmenu = LoadTexture("resources/bg/sappo.png");
    self->backui = LoadTexture("resources/bg/backui.png");

    self->imgstart = LoadTexture("resources/bg/start/start_00.png");
    self->imgstarthover = LoadTexture("resources/bg/start/start_01.png");

    self->imgrestart = LoadTexture("resources/bg/restart/restart_00.png");
    self->imgrestarthover = LoadTexture("resources/bg/restart/restart_01.png");

    self->imgbacktomenu = LoadTexture("resources/bg/menu/menu_00.png");
    self->imgbacktomenuhover = LoadTexture("resources/bg/menu/menu_01.png");

    self->imgquit = LoadTexture("resources/bg/quit/00_quit.png");
    self->imgquithover = LoadTexture("resources/bg/quit/01_quit.png");

    self->btnStart = (Rectangle){ centerX - 100, 300, 200, 50 };
    self->btnQuit = (Rectangle){ centerX - 100, 370, 200, 50 };
    self->btnRestart = (Rectangle){ centerX - 100, 300, 200, 50 };
    self->btnRestartPause = (Rectangle){ centerX - 100, 300, 200, 50 };
    self->btnBackToMenu = (Rectangle){ centerX - 100, 400, 200, 50 };
    self->btnBackToMenuWin = (Rectangle){ centerX - 100, 400, 200, 50 };
}

// ===================== ATUALIZAÇÃO DAS TELAS =====================
static void screen_update(ScreenManager *self, GameScreen *next_screen) {
    switch (self->current) {
        case SCREEN_MENU:
            if (ButtonClicked(self->btnStart)) *next_screen = SCREEN_GAMEPLAY;
            if (ButtonClicked(self->btnQuit)) CloseWindow();
            break;
        case SCREEN_PAUSE:
            if (ButtonClicked(self->btnRestartPause)) {
                reset_game_state();
                *next_screen = SCREEN_GAMEPLAY;
            }
            if (ButtonClicked(self->btnBackToMenu)) *next_screen = SCREEN_MENU;
            break;
        case SCREEN_WIN:
            if (ButtonClicked(self->btnBackToMenuWin)) *next_screen = SCREEN_MENU;
            break;
        case SCREEN_GAME_OVER:
            if (ButtonClicked(self->btnRestart)) {
                reset_game_state();
                *next_screen = SCREEN_GAMEPLAY;
            }
            if (ButtonClicked(self->btnBackToMenu)) *next_screen = SCREEN_MENU;
            break;
        default:
            break;
    }
}

// ===================== DESENHO DAS TELAS =====================
static void screen_draw(const ScreenManager *self) {
    switch (self->current) {
        case SCREEN_MENU: {
            DrawTexture(self->backmenu, 0, 0, WHITE);
            DrawTexture(
                CheckCollisionPointRec(GetMousePosition(), self->btnStart) ? self->imgstarthover : self->imgstart,
                self->btnStart.x, self->btnStart.y, WHITE
            );
            DrawTexture(
                CheckCollisionPointRec(GetMousePosition(), self->btnQuit) ? self->imgquithover : self->imgquit,
                self->btnQuit.x, self->btnQuit.y, WHITE
            );
            break;
        }
        case SCREEN_WIN: {
            int screenWidth = GetScreenWidth();

            DrawTexture(self->backui, 0, 0, WHITE);

            // Título centralizado
            const char *title = "VITORIA!";
            int titleFontSize = 40;
            int titleWidth = MeasureTextEx(self->font, title, titleFontSize, 2).x;
            DrawTextEx(
                self->font,
                title,
                (Vector2){ (screenWidth - titleWidth) / 2, 80 },
                titleFontSize, 2, (Color){164, 142, 105, 255 }
            );

            // Subtítulo centralizado
            const char *subtitle = "Todos os sapos chegaram em casa!";
            int subtitleFontSize = 15;
            Vector2 subtitleSize = MeasureTextEx(self->font, subtitle, subtitleFontSize, 1);
            float subtitleX = (screenWidth - subtitleSize.x) / 2.0f;
            if (subtitleX < 0) subtitleX = 0;
            Color subtitleShadow = (Color){ light.r, light.g, light.b, light.a - 100 };
            DrawTextEx(
                self->font,
                subtitle,
                (Vector2){ subtitleX, 130 },
                subtitleFontSize, 1, subtitleShadow
            );

            // Pontuação centralizada
            char score_text[64];
            sprintf(score_text, "PONTOS: %d", self->final_score);
            int scoreFontSize = 24;
            Vector2 scoreSize = MeasureTextEx(self->font, score_text, scoreFontSize, 1);
            DrawTextEx(
                self->font,
                score_text,
                (Vector2){ (screenWidth - scoreSize.x) / 2.0f, 180 },
                scoreFontSize, 1, light
            );

            // Tempo centralizado
            char time_text[64];
            sprintf(time_text, "TEMPO: %.2f segundos", self->total_time);
            int timeFontSize = 24;
            Vector2 timeSize = MeasureTextEx(self->font, time_text, timeFontSize, 1);
            DrawTextEx(
                self->font,
                time_text,
                (Vector2){ (screenWidth - timeSize.x) / 2.0f, 210 },
                timeFontSize, 1, light
            );

            // Botão Menu centralizado
            DrawTexture(
                CheckCollisionPointRec(GetMousePosition(), self->btnBackToMenu) ? self->imgbacktomenuhover : self->imgbacktomenu,
                self->btnBackToMenu.x, self->btnBackToMenu.y, WHITE
            );
            break;
        }
        case SCREEN_PAUSE: {
            DrawTextureEx(self->backui, (Vector2){0, 0}, 0.0f, 1.0f, WHITE);
            const char *pauseText = "PAUSADO!";
            int pauseFontSize = 50;
            int pauseTextX = GetScreenWidth() / 2 - MeasureText(pauseText, pauseFontSize) / 2;
            DrawTextEx(self->font, pauseText, (Vector2){pauseTextX, 150}, pauseFontSize, 0, light);

            // Botão Restart centralizado
            float btnRestartWidth = self->imgrestart.width;
            float btnRestartX = (GetScreenWidth() - btnRestartWidth) / 2.0f;
            DrawTexture(
                CheckCollisionPointRec(GetMousePosition(), self->btnRestartPause) ? self->imgrestarthover : self->imgrestart,
                btnRestartX, self->btnRestartPause.y, WHITE
            );

            // Botão Menu centralizado
            float btnWidth = self->imgbacktomenu.width;
            float btnX = (GetScreenWidth() - btnWidth) / 2.0f;
            DrawTexture(
                CheckCollisionPointRec(GetMousePosition(), self->btnBackToMenu) ? self->imgbacktomenuhover : self->imgbacktomenu,
                btnX, self->btnBackToMenu.y, WHITE
            );
            break;
        }
        case SCREEN_GAME_OVER: {
            int screen_width = GetScreenWidth();
            int screen_height = GetScreenHeight();
            
            DrawTexture(self->backui, 0, 0, WHITE);

            Vector2 center = {
                (screen_width - MeasureTextEx(self->font, "GAME OVER", 50, 0).x) / 2, 200
            };
            DrawTextEx(self->font, "!GAME OVER!", center, 50, 0, (Color){154, 132, 95, 255 });

            // Botão Restart
            DrawTexture(
                CheckCollisionPointRec(GetMousePosition(), self->btnRestart) ? self->imgrestarthover : self->imgrestart,
                self->btnRestart.x, self->btnRestart.y, WHITE
            );

            // Botão Menu
            DrawTexture(
                CheckCollisionPointRec(GetMousePosition(), self->btnBackToMenu) ? self->imgbacktomenuhover : self->imgbacktomenu,
                self->btnBackToMenu.x, self->btnBackToMenu.y, WHITE
            );
            break;
        }
        default:
            break;
    }
}

// ===================== CRIAÇÃO DO SCREEN MANAGER =====================
ScreenManager create_screen_manager() {
    ScreenManager s = {0};
    s.current = SCREEN_MENU;
    s.init = screen_init;
    s.update = screen_update;
    s.draw = screen_draw;
    return s;
}
