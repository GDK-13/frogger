# Frogger 2025

Um remake moderno do clássico Frogger, desenvolvido em C com a biblioteca [Raylib](https://www.raylib.com/). O objetivo é atravessar ruas movimentadas, evitar obstáculos e ocupar todas as casas no topo da tela!

---

## 🎮 Como Jogar

- **Setas direcionais**: Movem o sapo (↑, ↓, ←, →)
- **ENTER**: Pausa/retoma o jogo
- **ESC**: Fecha o jogo
- **F2**: Força Game Over (debug)
- **F3**: Força vitória (debug)
- **F4**: Mostra/oculta hitboxes (debug)

---

## 🕹️ Mecânicas

- **Pontuação**:
    - +10 pontos a cada linha (32 pixels) avançada para cima.
    - +200 pontos ao ocupar uma casa no topo.
    - +100 pontos extras ao ocupar uma casa com uma mosca ativa.
- **Vidas**:
    - Início com 5 vidas.
    - Perde 1 vida ao colidir com um carro ou cair na água.
    - Game Over ao perder todas as vidas.
- **Casas**:
    - 5 casas disponíveis no topo.
    - Ao chegar em uma casa livre, ela é ocupada e um sapo aparece nela.
    - Complete todas as casas para vencer.
- **Mosca bônus**:
    - Uma mosca aparece aleatoriamente em casas vazias por tempo limitado.
    - Se o jogador ocupar uma casa com a mosca ativa, ganha 100 pontos extras.
    - Após ser capturada ou expirar, a mosca entra em cooldown antes de reaparecer.
- **Cronômetro**:
    - O tempo de partida é exibido no canto inferior direito da tela.
    - O cronômetro para quando todas as casas são ocupadas ou ocorre Game Over.
- **Pause**:
    - O jogo pode ser pausado e retomado a qualquer momento com ENTER.
- **Debug**:
    - F2: Força Game Over.
    - F3: Força vitória.
    - F4: Mostra/oculta hitboxes.

---

## ⚙️ Como Compilar

### Pré-requisitos

- [Raylib](https://www.raylib.com/) instalada (versão 4.x recomendada)
- Compilador C (gcc, clang, etc.)

*O projeto possui **dois scripts de compilação automatizados** na raiz do repositório:*

- `build.sh` – para **Linux/macOS**
- `build.bat` – para **Windows**

---

### Linux/macOS

1. Dê permissão de execução ao script:
    ```sh
    chmod +x build.sh
    ```
2. Execute o script:
    ```sh
    ./build.sh
    ```
   O script irá:
   - Compilar todos os arquivos fonte do projeto.
   - Gerar o executável na pasta `game/bin/Debug`.
   - Se a compilação for bem-sucedida, o jogo será iniciado automaticamente.

---

### Windows (MSYS2/MinGW)

1. Abra o terminal MSYS2/MinGW na pasta do projeto.
2. Execute o script:
    ```sh
    build.bat
    ```
   O script irá:
   - Compilar o projeto usando o gcc do MSYS2/MinGW.
   - Gerar o executável na pasta `game/bin/Debug`.
   - Iniciar o jogo automaticamente se não houver erros.

---

### Observações sobre os scripts e a estrutura do projeto

- Cada script é específico para seu sistema operacional.
- Certifique-se de que o Raylib esteja corretamente instalado e configurado no seu sistema.
- Caso encontre erros de biblioteca não encontrada, verifique se as variáveis de ambiente do Raylib estão corretas.
- Se desejar apenas compilar (sem executar), edite o script correspondente e comente a linha final que executa o binário.

- Existem **duas pastas `resources/`**, uma para cada sistema operacional:
  - `game/bin/Debug/resources` – usada no Linux/macOS.
  - `game/resources` – usada no Windows.

> Certifique-se de que a pasta correta esteja sendo utilizada durante a execução do jogo, conforme seu sistema operacional.

- Após a compilação, **dois executáveis** são gerados na pasta `game/bin/Debug/`:
  - `sappoWINDOWS.exe` (para Windows)
  - `sappoLINUX` (para Linux/macOS)

> Execute o binário correspondente ao seu sistema operacional.

---

## 📦 Recursos

- **Sprites**: `resources/[sistema]/sprites/`
- **Fontes**: `resources/[sistema]/fontes/`
- **Sons**: `resources/[sistema]/sounds/`
- **Background e UI**: `resources/[sistema]/bg/`

---

## ✨ Créditos

- **Código**: Ingrid Kelly [(luzingrid)](https://github.com/luzingrid) e Kevin [(GDK-13)](https://github.com/GDK-13)
- **Biblioteca gráfica**: [Raylib](https://www.raylib.com/)
- **Sprites**: Kevin (GDK-13)
- **Fontes**: [*8_bit_fortress*](https://www.dafont.com/pt/8-bit-fortress.font?l[]=10) criada por Fortress Tech
- **Sons**:
    - Efeitos sonoros: Kevin (GDK-13) usando [jsrfx](https://sfxr.me/)
    - Música tema "Frog's Oath": Kevin (GDK-13) usando [beepbox.co](https://www.beepbox.co)
- **Template de setup Raylib/Premake**:
    - [game-premake](https://github.com/raylib-extras/game-premake/) por Jeffery Myers  
    - Veja o arquivo `READMERAYSET.md` para instruções detalhadas de configuração e licença.
    - **Atenção:** O código do template foi modificado para atender às necessidades deste projeto e **não pretende se passar pela versão original**.
    - **Agradecimento especial ao Jeffery Myers pelo template original!**

---

## 📋 Observações

- Projeto desenvolvido para fins didáticos e de aprendizado.
- Modifique, melhore e compartilhe à vontade!
- Para dúvidas ou sugestões, abra uma issue ou envie um pull request.
- **Atenção**: Todo o desenvolvimento e testes deste projeto foram realizados utilizando o Visual Studio Code. Não podemos garantir que o template e o projeto funcionarão corretamente em outras IDEs.
