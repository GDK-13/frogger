@echo off
setlocal

:: Cria a pasta bin/Debug
if not exist "game\bin\Debug" (
    mkdir "game\bin\Debug"
    if errorlevel 1 (
        echo Erro ao criar o diretório game\bin\Debug.
        exit /b 1
    )
)

:: Caminho de saída
set OUTPUT=game\bin\Debug\sappoWINDOWS.exe

:: Compilação com GCC (MinGW)
gcc -o %OUTPUT% ^
    game\src\main.c ^
    game\src\player.c ^
    game\src\animation.c ^
    game\src\car.c ^
    game\src\hud.c ^
    game\src\events.c ^
    game\src\trunk.c ^
    game\src\turtle.c ^
    game\src\screen.c ^
    -Igame\include -Igame\src ^
    -Lgame\bin\Debug ^
    -lraylib ^
    -std=c17 -g -DPLATFORM_DESKTOP

if errorlevel 1 (
    echo Build failed!
    exit /b 2
)

echo Build successful!
echo Running the game...
cd game\bin\Debug
if exist sappoWINDOWS.exe (
    sappoWINDOWS.exe
) else (
    echo Executável não encontrado.
    exit /b 3
)
