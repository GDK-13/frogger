#ifndef SCREEN_H
#define SCREEN_H

#include "raylib.h"
#include <stdbool.h>

// Estados possíveis do jogo
typedef enum GameScreen {
    SCREEN_MENU,
    SCREEN_GAMEPLAY,
    SCREEN_PAUSE,
    SCREEN_GAME_OVER,
    SCREEN_WIN
} GameScreen;

// Estrutura que gerencia as telas do jogo
typedef struct ScreenManager {
    GameScreen current;  // Tela atual do jogo

    // Botões usados nas diferentes telas
    Rectangle btnStart;           // Botão "Start" no menu
    Rectangle btnRestart;         // Botão "Restart" na tela de Game Over
    Rectangle btnBackToMenu;      // Botão "Menu" na tela de pausa
    Rectangle btnBackToMenuWin;   // Botão "Menu" na tela de vitória

    // Fonte usada nos textos das telas
    Font font;

    // Informações exibidas na tela de vitória
    int final_score;     // Pontuação final
    float total_time;    // Tempo total jogado

    // Métodos da estrutura
    void (*init)(struct ScreenManager *self);                              // Inicializa botões, fontes etc.
    void (*update)(struct ScreenManager *self, GameScreen *next_screen);   // Atualiza a lógica da tela atual
    void (*draw)(const struct ScreenManager *self);                        // Desenha a tela atual
} ScreenManager;

// Cria e retorna um ScreenManager com ponteiros já configurados
ScreenManager create_screen_manager();

// Função externa que reinicia o estado do jogo
extern void reset_game_state(void);

#endif
