# Registro de prompts e uso de IA

Este arquivo registra o uso de Codex no desenvolvimento do trabalho final. Nem
todos os prompts literais originais foram preservados; por isso, as entradas
abaixo sao reconstrucoes representativas, baseadas no historico de commits, no
estado atual dos arquivos e nas tarefas executadas. Em commits marcados como
`HUMANO + IA`, o Codex foi usado como apoio de implementacao, integracao,
depuracao, revisao ou polimento, com decisao e validacao humana.

A pasta temporaria `portas_abertas_reference/` foi usada apenas como referencia
local durante a integracao da versao expandida e nao deve ser versionada.

## Prompt 001

**Commit:** HUMANO + IA: adicionar tiro basico contra o Pe Grande
**Ferramenta:** Codex
**Objetivo:** implementar o primeiro fluxo de tiro e dano contra o inimigo.

```text
Ajude a adicionar uma mecanica basica de tiro contra o Pe Grande. O jogador
deve poder atirar com a shotgun, testar se o disparo acerta o inimigo e aplicar
dano. Mantenha a implementacao simples e compativel com a estrutura atual do
main.cpp e do modulo Bigfoot. Evite refatoracoes grandes; o objetivo e criar a
base para vida, dano e morte do inimigo.
```

## Prompt 002

**Commit:** HUMANO + IA: bloquear tiro em obstaculos
**Ferramenta:** Codex
**Objetivo:** impedir que disparos atravessem paredes ou objetos do cenario.

```text
Revise a logica de acerto da shotgun para que o tiro nao acerte o Pe Grande se
houver obstaculo entre a camera e o inimigo. Use as caixas de colisao do cenario
como bloqueadores do raio. Preserve o comportamento de tiro ja existente e
altere apenas o necessario para considerar linha de visao e distancia.
```

## Prompt 003

**Commit:** HUMANO + IA: adicionar recuo por curva de Bezier ao Pe Grande
**Ferramenta:** Codex
**Objetivo:** atender ao requisito de curva Bezier cubica na fuga apos tiro.

```text
Implemente uma fuga do Pe Grande apos levar tiro usando curva Bezier cubica.
A classe Bigfoot deve armazenar quatro pontos de controle, calcular a posicao
com De Casteljau e entrar em um estado Fleeing antes de voltar para Chasing.
Nao substitua a IA inteira; integre a curva ao fluxo atual de dano e movimento.
Inclua comentarios uteis explicando a Bezier e mantenha o movimento limitado
por frame para evitar teleporte.
```

## Prompt 004

**Commit:** IA: refinar mapa, texturas e movimentacao do Pe Grande
**Ferramenta:** Codex
**Objetivo:** melhorar o visual do cenario e a movimentacao do inimigo.

```text
Refine o mapa e a apresentacao visual do jogo: organize melhor o plano/cenario,
adicione texturas de grama e tijolos, ajuste os shaders quando necessario e
melhore a movimentacao do Pe Grande para parecer menos travada. Mantenha o jogo
compilavel e evite alterar mecanicas centrais que ja funcionam, como coleta,
safe zone e tiro.
```

## Prompt 005

**Commit:** HUMANO + IA: Correcao de bugs: corrigir colisao e retorno da fuga do Pe Grande
**Ferramenta:** Codex
**Objetivo:** corrigir bugs na fuga Bezier e na colisao do Pe Grande.

```text
Investigue bugs na movimentacao do Pe Grande apos tomar tiro. A fuga por Bezier
deve terminar corretamente e retornar para perseguicao, sem teleporte e sem
atravessar paredes. Corrija a colisao/sliding durante a fuga e durante a
perseguicao, preservando De Casteljau, os pontos de controle e o estado Fleeing.
Rode o build ao final.
```

## Prompt 006

**Commit:** Update .gitignore; HUMANO + IA: preparar integracao e adicionar assets Portas Abertas
**Ferramenta:** Codex
**Objetivo:** preparar o repositorio e adicionar apenas assets passivos.

```text
Prepare o repositorio oficial para integrar gradualmente a versao expandida
usando portas_abertas_reference/ apenas como referencia. Atualize o .gitignore
para impedir build, dist, bin, caches, arquivos temporarios e save de runtime.
Depois copie apenas assets passivos novos, como sons, shaders passivos, icone,
modelo e textura da lata Monster Ultra. Nao integrar codigo C++ nesta fase e
nao modificar portas_abertas_reference/.
```

## Prompt 007

**Commit:** HUMANO + IA: integrar base tecnica, gameplay e interface Portas Abertas
**Ferramenta:** Codex
**Objetivo:** trazer modulos auxiliares isolados e configurar o build.

```text
Integre modulos tecnicos isolados da versao Portas Abertas: audio, upgrades,
map_view e debug_flags. Adicione os headers e fontes correspondentes, ajuste o
CMakeLists.txt para compilar os novos .cpp, linke winmm no Windows se o audio
precisar e inclua o recurso de icone se for simples. Nao conectar ainda a loja
ou o fluxo principal se isso exigir mudancas grandes. O projeto deve continuar
compilando.
```

## Prompt 008

**Commit:** HUMANO + IA: integrar base tecnica, gameplay e interface Portas Abertas
**Ferramenta:** Codex
**Objetivo:** integrar menus, HUD, loja, progresso e fluxo principal.

```text
Agora integre a maior parte da experiencia Portas Abertas, sem substituir a
logica do Pe Grande. Traga menus, HUD, loja/upgrades, energia, dinheiro,
progresso, reset com confirmacao, textos de interface, fluxo de estados,
coletaveis, safe zone, mapa e renderizacao da lata Monster Ultra. Ajuste
player, camera, scene e shaders ativos somente quando necessario. Nao exibir
comandos de debug/cheat na interface, nao reiniciar com R durante a partida e
nao usar ESC para fechar a loja se ESC solta o mouse.
```

## Prompt 009

**Commit:** HUMANO + IA: integrar melhorias do Pe Grande preservando Bezier
**Ferramenta:** Codex
**Objetivo:** integrar melhorias do inimigo sem substituir a fuga oficial.

```text
Compare bigfoot.cpp e bigfoot.h oficiais com a versao de referencia e integre
somente melhorias compativeis. Preserve obrigatoriamente Lerp,
ComputeBezierPoint(), os pontos de controle bezier_p0..bezier_p3, StartFleeing
com Bezier cubica, De Casteljau, estado Fleeing, retorno para Chasing, limite
de movimento por frame e colisao/sliding. Pode trazer spawn melhor,
multiplicadores de dificuldade, pequenas melhorias de perseguicao, ataque e
funcoes chamadas pelo main.cpp atual, mas nao copie os arquivos por cima.
```

## Prompt 010

**Commit:** HUMANO + IA: finalizar integracao Portas Abertas com Spectator
**Ferramenta:** Codex
**Objetivo:** revisar a integracao final e fazer polimentos pequenos.

```text
Faca uma revisao final da integracao. Verifique se portas_abertas_reference/,
build, bin, dist, build-codex, .claude e data/save_prestige.txt nao entram no
commit. Revise caminhos de assets e shaders, veja se README, SPEC ou PROMPTS
precisam de atualizacao minima e rode o build. Como polimento, aumente
moderadamente o side_offset da fuga por Bezier para deixar a curva mais visivel,
sem alterar ComputeBezierPoint(), De Casteljau, limite por frame ou colisao.
```

## Prompt 011

**Commit:** HUMANO + IA: finalizar integracao Portas Abertas com Spectator
**Ferramenta:** Codex
**Objetivo:** integrar o modo Spectator/IA da versao expandida.

```text
Integre o modo Spectator ativado com L. Quando ativo durante a partida, o player
deve ser controlado por IA: buscar latas, depois ir para a safe zone, mirar no
Pe Grande quando visivel, atirar automaticamente quando alinhado e tentar se
afastar se o inimigo estiver perto. A IA deve usar a mesma colisao/coleta do
player normal. Vitoria em Spectator deve avancar para o proximo nivel apos cerca
de 2 segundos; derrota deve reiniciar o mesmo nivel apos cerca de 2 segundos,
mantendo Spectator ativo. Mova reload de shaders para F5 se necessario.
```

## Prompt 012

**Commit:** HUMANO + IA: finalizar integracao Portas Abertas com Spectator
**Ferramenta:** Codex
**Objetivo:** balancear a velocidade final do Pe Grande sem quebrar a Bezier.

```text
A velocidade default do Pe Grande esta baixa demais para a versao final.
Aumente no minimo 2x a velocidade base de perseguicao para ele ser ameacador
desde o inicio. Se ApplyDifficultyMultipliers() ou o cap de progressao deixarem
niveis altos absurdos, ajuste o multiplicador/cap com cuidado. Nao altere
ComputeBezierPoint(), nao remova De Casteljau e mantenha a fuga por Bezier,
limite por frame e colisao/sliding funcionando.
```
