# Pe Grande do Vale

Jogo em primeira pessoa desenvolvido para o trabalho final de Computacao
Grafica e Visualizacao I (INF01047) - INF/UFRGS.

## Build

```powershell
cmake --preset default-config
cmake --build --preset default-build
```

## Executar

```powershell
.\bin\Debug\main.exe
```

O executavel espera encontrar os assets em `data/` e os shaders ativos em
`data/shaders/`.

## Controles principais

- WASD: mover
- Mouse: olhar
- Shift: correr
- Botao esquerdo: atirar
- U: abrir loja de upgrades
- M: abrir mapa durante a partida
- Space: iniciar ou selecionar
- R: voltar ao menu apos vitoria ou derrota
- ESC: soltar o mouse

O progresso de runtime e salvo em `data/save_prestige.txt`. Esse arquivo nao
deve ser versionado.
