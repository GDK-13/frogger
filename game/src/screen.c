#include "screen.h"
#include <stdio.h>

// Verifica se o botão representado por 'rect' foi clicado com o mouse
static bool ButtonClicked(Rectangle rect) {
    return CheckCollisionPointRec(GetMousePosition(), rect) &&
           IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Inicializa as posições dos botões e carrega a fonte usada nas telas
static void screen_init(ScreenManager *self) {
    int centerX = GetScreenWidth() / 2;

    self->btnStart = (Rectangle){ centerX - 100, 300, 200, 50 };
    self->btnRestart = (Rectangle){ centerX - 100, 300, 200, 50 };
    self->btnBackToMenu = (Rectangle){ centerX - 100, 400, 200, 50 };
    self->btnBackToMenuWin = (Rectangle){ centerX - 100, 400, 200, 50 };

    self->font =LoadFontEx("resources/fontes/8_bit_fortress/8-bit-fortress.ttf", 32, NULL, 0);
    if (self->font.baseSize == 0) {
        printf("Erro: Não foi possível carregar a fonte 8-bit\n");
        CloseWindow();
    }

}

// Atualiza o estado do jogo baseado na tela atual e ações do jogador
static void screen_update(ScreenManager *self, GameScreen *next_screen) {
    switch (self->current) {
        case SCREEN_MENU:
            if (ButtonClicked(self->btnStart)) {
                *next_screen = SCREEN_GAMEPLAY;
            }
            break;

        case SCREEN_PAUSE:
            if (ButtonClicked(self->btnBackToMenu)) {
                *next_screen = SCREEN_MENU;
            }
            break;

        case SCREEN_WIN:
            if (ButtonClicked(self->btnBackToMenuWin)) {
                *next_screen = SCREEN_MENU;
            }
            break;

        default:
            break;
    }
}

// Desenha a interface gráfica para a tela atual
static void screen_draw(const ScreenManager *self) {

    switch (self->current) {
        case SCREEN_MENU: {
            // Desenha o título do menu principal
            const char *title = "MENU PRINCIPAL";
            int titleFontSize = 40;
            int titleX = GetScreenWidth() / 2 - MeasureText(title, titleFontSize) / 2;
            DrawTextEx(self->font, title, (Vector2){titleX, 150}, titleFontSize, 2, WHITE);

            // Botão START com efeito hover
            Color btnColor = CheckCollisionPointRec(GetMousePosition(), self->btnStart) ? GRAY : DARKGRAY;
            DrawRectangleRec(self->btnStart, btnColor);

            const char *btnText = "START";
            int btnFontSize = 20;
            int btnTextX = self->btnStart.x + self->btnStart.width / 2 - MeasureText(btnText, btnFontSize) / 2;
            int btnTextY = self->btnStart.y + 15;
            DrawText(btnText, btnTextX, btnTextY, btnFontSize, WHITE);
            break;
        }

        case SCREEN_WIN: {
            int screenCenter = GetScreenWidth() / 2;

            // Mensagem de vitória
            DrawTextEx(self->font, "VITÓRIA!", (Vector2){screenCenter - 100, 100}, 40, 2, GREEN);

            // Exibe pontuação final
            char score_text[64];
            sprintf(score_text, "PONTOS: %d", self->final_score);
            DrawTextEx(self->font, score_text, (Vector2){screenCenter - 80, 160}, 24, 1, WHITE);

            // Exibe tempo total
            char time_text[64];
            sprintf(time_text, "TEMPO: %.2f segundos", self->total_time);
            DrawTextEx(self->font, time_text, (Vector2){screenCenter - 100, 190}, 24, 1, WHITE);

            // Botão MENU para voltar
            DrawRectangleRec(self->btnBackToMenuWin, DARKGRAY);
            DrawText("MENU", self->btnBackToMenuWin.x + 70, self->btnBackToMenuWin.y + 15, 20, WHITE);
            break;
        }

        case SCREEN_PAUSE: {
            // Texto de pausa centralizado
            const char *pauseText = "PAUSADO";
            int pauseFontSize = 40;
            int pauseTextX = GetScreenWidth() / 2 - MeasureText(pauseText, pauseFontSize) / 2;
            DrawTextEx(self->font, pauseText, (Vector2){pauseTextX, 150}, pauseFontSize, 2, YELLOW);

            // Botão MENU com efeito hover
            Color btnColor = CheckCollisionPointRec(GetMousePosition(), self->btnBackToMenu) ? GRAY : DARKGRAY;
            DrawRectangleRec(self->btnBackToMenu, btnColor);

            int btnFontSize = 20;
            int btnTextX = self->btnBackToMenu.x + self->btnBackToMenu.width / 2 - MeasureText("MENU", btnFontSize) / 2;
            int btnTextY = self->btnBackToMenu.y + 15;
            DrawText("MENU", btnTextX, btnTextY, btnFontSize, WHITE);
            break;
        }

        default:
            break;
    }
}

// Função para criar e inicializar o ScreenManager com seus métodos
ScreenManager create_screen_manager() {
    ScreenManager s = {0};
    s.current = SCREEN_MENU;
    s.init = screen_init;
    s.update = screen_update;
    s.draw = screen_draw;
    return s;
}
