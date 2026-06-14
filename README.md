# Pé Grande do Vale

## Computação Gráfica e Visualização I (INF01047) - INF/UFRGS

## Integrantes

- Vinícius Gross Castro
- João Vitor de Medeiros

## Descrição

**Pé Grande do Vale** é um jogo de terror/ação em primeira pessoa feito com OpenGL. O jogo simula a experiência comum de um aluno no Campus do Vale da UFRGS durante a noite. Ele deve coletar latas de energético e precisa escapar para zona segura (parada de ônibus) enquanto é perseguido pelo Pé Grande. Para sobreviver, o jogador pode usar uma shotgun para afastar o monstro, ou até mesmo derrotá-lo. O jogador pode vencer ao escapar, ou derrotar o Pé Grande. Com múltiplos níveis e upgrades, cada sessão é uma aventura que se torna cada vez mais difícil e intensa.

A aplicação possui um HUD simples, mecânica de coleta de itens, colisão, tiro, dano e vida dos inimigos, loja de upgrades, áudio, música, mapa, progressão de níveis e modo Spectator/IA.

## Contribuições

Vinícius trabalhou principalmente com a base do projeto: implementação de gameplay, Pé Grande, tiro, dano, colisões, fuga por curva Bézier, HUD, coleta, IA, zona segura, animações, integração de versões e documentação.

João Vitor contribuiu principalmente com a escolha e implementação de modelos 3D, game design, sistema de upgrades, ajustes de assets/modelos, remodelagem do mapa, implementação de interiores, ajustes na IA, iluminação, sombras, implementação de itens diversos como bancos e postes, testes, câmera do mapa e do Pé Grande e criação do vídeo de demonstração.

## Uso de IA

Usamos principalmente ChatGPT/OpenAI Codex, como apoio no desenvolvimento, principalmente após o núcleo inicial do jogo já estar implementado. Usamos para auxiliar implementação de mecânicas, modelo do pé grande, árvores e mapa inicial. Além da lógica para o modo telespectador/IA.


O arquivo [`PROMPTS.md`](PROMPTS.md) contém os prompts utilizados durante o desenvolvimento.

## Imagens

<img width="1908" height="982" alt="image" src="https://github.com/user-attachments/assets/9812750d-c6b8-430e-844d-80ce861d0492" />

<img width="1273" height="712" alt="image" src="https://github.com/user-attachments/assets/f7a75b46-51c9-4d01-b8fb-12ef79a695c3" />

## Vídeo

https://drive.google.com/file/d/1FXKXMqFqBsQiGiyu4fxYPhIzHbkiAHFO/view?usp=sharing

## Controles

| Controle | Ação |
|---|---|
| W, A, S, D | mover |
| Mouse | olhar |
| Botão esquerdo | atirar |
| Shift | correr |
| U | abrir loja de upgrades |
| W / S na loja | navegar nos upgrades |
| Space na loja | comprar upgrade |
| M | abrir mapa |
| Alt | Modo "Visão Do Pé Grande" |
| L | ativar/desativar Spectator -> IA normal |
| K | ativar/desativar Spectator -> IA agressiva |
| ESC | desfocar a aplicação (libera mouse) |
| R | voltar ao menu após vitória/derrota |

## Objetivo do jogo

1. Coletar todas as latas de energético.
2. Sobreviver ao Pé Grande.
3. Usar a shotgun para afastar o inimigo.
4. Entrar na zona segura após coletar os itens.


## Disponível online!
Uma versão com apenas o executável e essencias pode ser baixada de graça através deste link

https://bitmancer-games.itch.io/jogo-portas-abertas

<img width="1014" height="813" alt="image" src="https://github.com/user-attachments/assets/ac2c87ab-abef-4b49-a873-00f3c6fff92d" />


## Compilação

Pré-requisitos:

- CMake
- Ninja
- MinGW-w64
- OpenGL 3.3 compatível

Na raiz do repositório, execute:

```powershell
cmake --preset default-config
cmake --build --preset default-build
```

O executável vai ser gerado em: bin/Debug/main.exe

Execute o jogo a partir desta mesma pasta para não causar problemas com os caminhos relativos dos assets

```powershell
cd bin/Debug
./main.exe
```

### Vídeo de apresentação
