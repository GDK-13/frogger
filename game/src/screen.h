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
    Rectangle btnQuit;            // Botão "Quit" no menu
    Rectangle btnRestart;         // Botão "Restart" na tela de Game Over
    Rectangle btnRestartPause;    // Botão "Restart" na tela de pausa
    Rectangle btnBackToMenu;      // Botão "Menu" na tela de pausa e game over
    Rectangle btnBackToMenuWin;   // Botão "Menu" na tela de vitória

    // Fonte usada nos textos das telas
    Font font;

    // Imagens das telas e botões
    Texture2D backmenu;           // Imagem de fundo do menu
    Texture2D backui;             // Imagem de fundo das telas de UI

    Texture2D imgstart;           // Imagem do botão "Start"
    Texture2D imgstarthover;      // Imagem do botão "Start" quando hover

    Texture2D imgrestart;         // Imagem do botão "Restart"
    Texture2D imgrestarthover;    // Imagem do botão "Restart" quando hover

    Texture2D imgbacktomenu;      // Imagem do botão "Menu"
    Texture2D imgbacktomenuhover; // Imagem do botão "Menu" quando hover

    Texture2D imgquit;           // Imagem do botão "Quit"
    Texture2D imgquithover;      // Imagem do botão "Quit" quando hover

    // Informações exibidas na tela de vitória
    int final_score;     // Pontuação final
    float total_time;    // Tempo total jogado

    // Métodos da estrutura
    void (*init)(struct ScreenManager *self);                              // Inicializa botões, fontes etc.
    void (*update)(struct ScreenManager *self, GameScreen *next_screen);   // Atualiza a lógica da tela atual
    void (*draw)(const struct ScreenManager *self);                        // Desenha a tela atual
} ScreenManager;

// Cria e retorna um ScreenManager com ponteiros já configurados
ScreenManager create_screen_manager(void);

// Função externa que reinicia o estado do jogo
extern void reset_game_state(void);

#endif
