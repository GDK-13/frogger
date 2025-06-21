#include "hud.h"      // Inclui o cabeçalho do HUD para acessar a declaração da função
#include "events.h"  // Necessário para acessar a estrutura Time usada no HUD
#include "raylib.h"   // Necessário para usar Texture2D, Vector2, DrawTexture, etc.
#include <stdio.h>    // Necessário para usar sprintf

// Função responsável por desenhar as informações do HUD na tela
void draw_hud(Font font, int vidas, int score, Texture2D life, Time *timer) {
    // --- Definição de cores do HUD ---
    Color light = (Color){ 205, 178, 171, 255 }; // Cor clara para textos e fundo
    Color dark  = (Color){ 46, 22, 51, 255 };    // Cor escura para o número do score


    // --- Estrutura e inicialização dos ícones de vida ---
    typedef struct life_icon {
        Texture2D texture; // Textura do ícone de vida
        Vector2 position;  // Posição onde o ícone será desenhado
    } LifeIcon;

    LifeIcon icon[5];
    for (int i = 0; i < 5; i++) {
        icon[i].texture = life; // Atribui a textura do ícone de vida
    }

    // --- Desenho dos ícones de vida ---
    for (int i = 0; i < vidas; i++) {
        icon[i].position = (Vector2){ i * 32, 480 }; // Calcula a posição de cada ícone
        DrawTexture(icon[i].texture, icon[i].position.x, icon[i].position.y, WHITE); // Desenha o ícone
    }

    // --- Parâmetros para centralizar o score no topo ---
    float font_size = 24;                // Tamanho da fonte usada no HUD
    int screen_width = GetScreenWidth(); // Largura da tela para centralização

    // --- Preparação do texto do score ---
    const char *score_label = "SCORE";  // Texto do label do score
    char score_number[32];               // Buffer para o número do score
    sprintf(score_number, "%d", score);  // Converte o valor inteiro do score para string

    // Mede o tamanho do texto "SCORE:" e do número do score para centralizar
    Vector2 label_size  = MeasureTextEx(font, score_label, font_size, 0);
    Vector2 number_size = MeasureTextEx(font, score_number, font_size, 0);

    // Calcula a posição centralizada para o label e para o número
    Vector2 label_pos  = { (screen_width - label_size.x+2) / 2, 1 }; // Posição do "SCORE" no topo
    Vector2 number_pos = { (screen_width - number_size.x+2) / 2, 34 }; // Número logo abaixo

    // --- Desenho do score ---
    DrawTextEx(font, score_label, label_pos, font_size, 0, light); // Desenha o texto "SCORE:" centralizado no topo
    DrawRectangle(179, 30, label_size.x , 32, light);              // Desenha um ´retângulo claro atrás do número do score para destaque
    DrawTextEx(font, score_number, number_pos, font_size, 0, dark); // Desenha o número do score centralizado abaixo do label, usando cor escura

    // --- Instruções no HUD ---

    // Lado esquerdo: ESC e SAIR
    DrawTextEx(font, "ESC", (Vector2){ 2, 1 }, font_size, 0, light); 
    DrawTextEx(font, "SAIR", (Vector2){ 2, 32 }, font_size, 0, light);

    // Lado direito: ENTER e PAUSAR
    Vector2 enter_size = MeasureTextEx(font, "ENTER", font_size, 0);
    Vector2 pausar_size = MeasureTextEx(font, "PAUSE", font_size, 0);

    Vector2 enter_pos = { screen_width - enter_size.x, 1 }; // Posição do texto "ENTER"
    Vector2 pausar_pos = { screen_width - pausar_size.x, 32 }; // Posição do texto "PAUSAR"

    DrawTextEx(font, "ENTER", enter_pos, font_size, 0, light);
    DrawTextEx(font, "PAUSE", pausar_pos, font_size, 0, light);

    // --- Cronômetro no canto inferior direito ---

    
    
    char timer_text[16];
    sprintf(timer_text, "%02d %02d", timer->minutes, timer->seconds); // Formata o tempo do cronômetro como MM:SS
    Vector2 timer_size = MeasureTextEx(font, timer_text, 20, 0);
    Vector2 timer_position = { GetScreenWidth() - timer_size.x - 13, 484 }; // Canto inferior direito
    DrawRectangle(timer_position.x - 4 , timer_position.y - 2, timer_size.x +20 , 28, light);

    DrawRectangle(timer_position.x + 36 , timer_position.y + 16, 4 , 4 , dark);
    DrawRectangle(timer_position.x + 36 , timer_position.y + 5, 4 , 4 , dark);

    DrawTextEx(font, timer_text, timer_position, font_size, 0, dark);

    DrawTextEx(font, "TIMER", (Vector2){ timer_position.x - 90, timer_position.y }, font_size, 0, light); // Desenha o texto "TIME" ao lado do cronômetro
}

void pause_game(Font font) {
 int screen_width = GetScreenWidth();
            int screen_height = GetScreenHeight();
            DrawRectangle(0, 0, screen_width, screen_height, (Color){ 46, 22, 51, 170 });
            const char *pause_text = "PAUSADO";
            Vector2 pause_size = MeasureTextEx(font, pause_text, 40, 0);
            Vector2 pause_position = { (screen_width - pause_size.x) / 2, (screen_height - pause_size.y) / 2 };
            DrawTextEx(font, pause_text, pause_position, 40, 0, (Color){205, 178, 171, 255});
}