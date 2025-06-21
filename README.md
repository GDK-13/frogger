# Frogger 2025 

Um remake moderno do clássico Frogger, desenvolvido em C utilizando a biblioteca [Raylib](https://www.raylib.com/). O objetivo é atravessar ruas movimentadas, evitar obstáculos e ocupar todas as casas no topo da tela!

---

## 🎮 Como Jogar

- **Setas direcionais**: Movem o sapo (↑, ↓, ←, →)
- **ENTER**: Reinicia o jogo
- **ESC**: Encerra o jogo
- **PAUSAR**: 

---

## 🕹️ Mecânicas

- **Pontuação**:  
    - +10 pontos a cada linha (32 pixels) avançada para cima.
    - +200 pontos ao ocupar uma casa no topo.
- **Vidas**:  
    - Início com 5 vidas.
    - Perde 1 vida ao colidir com um carro.
    - Game Over ao perder todas as vidas.
- **Casas**:  
    - 5 casas disponíveis no topo.
    - Ao chegar em uma casa livre, ela é ocupada e um sapo aparece nela.
    - Complete todas as casas para vencer.

---

## ⚙️ Como Compilar

### Pré-requisitos

- [Raylib](https://www.raylib.com/) instalada
- Compilador C (gcc, clang, etc.)

O projeto já possui um script de compilação automatizado chamado `build.sh` na raiz do repositório.

### Linux/macOS

Abra o terminal na pasta do projeto e execute:

```sh
chmod +x build.sh
./build.sh
```

### Windows (MSYS2/MinGW)

Abra o terminal MSYS2 na pasta do projeto e execute:

```sh
sh build.sh
```

O script irá compilar o projeto e, se a compilação for bem-sucedida, já executará o jogo automaticamente.

---

## 📦 Recursos

- **Sprites**: `resources/sprites/`
- **Fontes**: `resources/fontes/`
- **Sons**: `resources/sounds/`
- **Background**: `resources/bg/`

---

## ✨ Créditos

- **Código**: Ingrid Kelly [(luzingrid)](https://github.com/luzingrid) e Kevin [(GDK-13)](https://github.com/GDK-13)
- **Biblioteca gráfica**: [Raylib](https://www.raylib.com/)
- **Sprites**: Kevin (GDK-13)
- **Fontes**: [*8_bit_fortress*](https://www.dafont.com/pt/8-bit-fortress.font?l[]=10) criada por Fortress Tech 
- **Sons**:  
    - Efeitos sonoros: Kevin (GDK-13) usando: [jsrfx](https://sfxr.me/)
    - Música tema "Frog's Oath": Kevin (GDK-13) usando: [beepbox.co](https://www.beepbox.co)
- **Template de setup Raylib/Premake**:  
    - [game-premake](https://github.com/raylib-extras/game-premake/) por Jeffery Myers  
    - Veja o arquivo `READMERAYSET.md` para instruções detalhadas de configuração e licença.
    - **Atenção:** A versão dos códigos referidos em `READMERAYSET.md` foi modificada por nós para atender às necessidades deste projeto e **não pretende se passar pela original**, que pode ser encontrada no link referido acima (game-premake).
    - **Agradecimento especial ao Jeffery Myers por disponibilizar o template original!**

---

## 📋 Observações

- Projeto desenvolvido para fins didáticos e de aprendizado.
- Modifique, melhore e compartilhe à vontade!
- Para dúvidas ou sugestões, abra uma issue ou envie um pull request.

