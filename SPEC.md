# Especificação da Implementação


## Integrantes da dupla

- **Aluno 1 - Nome**: <mark>`Vinícius Gross Castro`</mark>
- **Aluno 1 - Cartão UFRGS**: <mark>`00324541`</mark>

- **Aluno 2 - Nome**: <mark>`João Vitor de Medeiros`</mark>
- **Aluno 2 - Cartão UFRGS**: <mark>`00270044`</mark>

## Detalhes do que será implementado

- **Título do trabalho**: <mark>`Pé Grande do Vale`</mark>
- **Parágrafo curto descrevendo o que será implementado**:

    Iremos criar um jogo de terror em primeira pessoa, inspirado no trecho do jogo Bigfoot Forest. Ao contrário do jogo, que representa uma pequena cidade, o cenário tentará simular uma simplificação da parte de cima do Campus do Vale durante a noite, com prédios, árvores, caminhos, veículos e cercas. No jogo original, o jogador coleta galões de combustível para poder escapar e usa uma shotgun para se defender do Pé Grande. Porém na nossa versão, o jogador deverá coletar latas de energético. A parte da shotgun permanecerá igual.

## Especificação visual

### Vídeo - Link

> [!IMPORTANT]
> - Coloque aqui um link para um vídeo que mostre a aplicação gráfica
>   de referência que você vai implementar. **Sua implementação deverá
>   ser o mais parecido possível com o que é mostrado no vídeo (mais
>   detalhes abaixo).**
> - **Você não pode escolher como referência: (1) algum trabalho realizado
>   por outros alunos desta disciplina, em semestres anteriores. (2) Minecraft.**
> - Por exemplo, você pode colocar um vídeo de um jogo que você gosta,
>   e seu trabalho final será uma re-implementação do jogo.
> - O vídeo pode ser um link para YouTube, Google Drive, ou arquivo mp4 dentro
>   do próprio repositório. Mas, garanta que qualquer um tenha
>   permissão de acesso ao vídeo através deste link.

<mark>`https://www.youtube.com/watch?v=aYYn4R52tXs`</mark>

### Vídeo - Timestamp

> [!IMPORTANT]
> - Coloque aqui um **intervalo de ~30 segundos** do vídeo acima, que
>   será a base de comparação para avaliar se o seu trabalho final
>   conseguiu ou não reproduzir a referência.

- **Timestamp inicial**: <mark>`0:38`</mark>
- **Timestamp final**: <mark>`1:08`</mark>

### Imagens

> [!IMPORTANT]
> - Coloque aqui **três imagens** capturadas do vídeo acima, que você
>   irá usar como ilustração para as explicações que vêm abaixo.

<img width="1263" height="710" alt="image" src="https://github.com/user-attachments/assets/d7432df8-8146-4759-a9d8-ccee53b5da32" />
<img width="1256" height="708" alt="image" src="https://github.com/user-attachments/assets/fad15b61-1cf4-460f-889c-b83eb53115e2" />
<img width="1263" height="701" alt="image" src="https://github.com/user-attachments/assets/0668a33a-c3b7-42d3-93ee-e7a67f539ad0" />




## Especificação textual

Para cada um dos requisitos abaixo (detalhados no [Enunciado do Trabalho final - Moodle](https://moodle.ufrgs.br/mod/assign/view.php?id=6018620)), escreva um parágrafo **curto** explicando como este requisito será atendido, apontando itens específicos do vídeo/imagens que você incluiu acima que atendem estes requisitos.

### Malhas poligonais complexas

Iremos incluir malhas poligonais 3D para representar os prédios, cercas, árvores, terreno (que será plano, a princípio), latas de energético, veículos, o Pé Grande e o jogador, que serão carregados como modelos tridimensionais.

### Transformações geométricas controladas pelo usuário

O jogador poderá se movimentar pelo cenário e orientar a câmera usando teclado e mouse. O jogador também poderá atirar com a arma, que resultará em um sutil efeito de coice. E também poderá coletar a lata de energético, que resultará em a mesma desaparecendo do cenário. Haverá uma janela pequena para instruir o jogador sobre os controles da aplicação.

### Diferentes tipos de câmeras

A câmera principal será em primeira pessoa, controlada pelo jogador. Além dela, será implementada uma câmera do Pé Grande, atrelada ao mesmo, que o jogador não poderá controlar.

### Instâncias de objetos
O jogo terá múltiplas instâncias de alguns modelos como árvores, cercas, edifícios e latas. Cada instância poderá ter uma posição, rotação e escala diferente. Modelos como o do jogador, Pé Grande e arma terão apenas uma instância.

### Testes de intersecção

Os testes de intersecção impedirão que o jogador atravesse edifícios, cercas e outros obstáculos do cenário. Além de delimitar os limites do cenário, pelos quais o jogador não poderá passar. Os testes também serão úteis para detectar o contato com o Pé Grande, e se houve intersecção entre o disparo da shotgun e o Pé Grande, permitindo causar dano.

### Modelos de Iluminação em todos os objetos

Os objetos da cena serão renderizados com iluminação não trivial, com o objetivo de reproduzir o ambiente noturno do jogo. A cena terá iluminação baixa, provida, a princípio, por um ponto único que atuará como iluminação global.

> Comentário Professor: o jogo orignal é bastante "escuro", o que dificulta ver os detalhes visuais. Sugir também implementarem um modo "dia" do jogo, onde a iluminação fica como se fosse um dia de sol, para conseguir ver melhor os detalhes do jogo (principalmente para a apresentação final).

### Mapeamento de texturas em todos os objetos

Cada objeto terá sua própria textura. Em caso de modelos como o Pé Grande, a arma, etc, a textura será definida pelo modelo do objeto. Casos como árvores, estradas, edifícios, serão usadas imagens de textura, como grama, asfalto, tijolo, concreto, etc, mantendo o estilo parecido com o que foi mostrado no vídeo, mas ao mesmo tempo procurando reproduzir o cenário proposto.

### Movimentação com curva Bézier cúbica

A curva de Bézier será utilizada na animação de recuo do Pé Grande após ele ser atingido pela shotgun. Após o acerto, o inimigo se distanciará do jogador seguindo uma trajetória curva, antes de voltar ao comportamento padrão.

### Animações baseadas no tempo ($\Delta t$)
Todas as movimentações e animações utilizarão o tempo entre quadros (Delta t) para que a velocidade das animações não dependa da taxa de quadros da máquina. Animações que serão implementadas incluem deslocamento do jogador, movimentação do Pé Grande, leve animação de recuo da arma e recuo do Pé Grande. Também podem vir a ser incluídas: animação de movimentação do modelo do Pé Grande e rotação em torno do próprio eixo das latas.

## Limitações esperadas

> [!IMPORTANT]
> - Coloque aqui uma lista de detalhes visuais ou de interação que
>   aparecem no vídeo e/ou imagens acima, mas que você **não pretende
>   implementar** ou que você **irá implementar parcialmente**.
> - Para cada item, **explique por que** não será implementado ou por
>   que será implementado parcialmente.

- A inteligência artificial do Pé Grande será simplificada: ele irá perseguir o jogador e reagir a tiros, mas não terá comportamento avançado ou animações complexas.
- O sistema de arma será simplificado. Não serão implementados sistemas de munição, recarga ou equipar/desequipar.
- Os modelos e texturas não vão ser os mesmos do jogo de referência. Serão usados modelos condizentes com o projeto, as mudanças previstas e a proposta de reproduzir o Campus do Vale.
- A câmera do Pé Grande não aparece no vídeo de referência, mas será adicionada para cumprir o requisito de diferentes tipos de câmera.
- A animação de recuo por curva de Bézier será aplicada apenas quando o Pé Grande for atingido. O vídeo de referência mostra o confronto entre o jogador e o Pé Grande, mas não apresenta essa funcionalidade. Essa adaptação será incluída com o propósito de atender o requisito de movimentação com curva Bézier cúbica.
- Animações da câmera de subir e descer a cada passo não serão implementadas.
- Efeitos de partícula de sangue ao atingir o Pé Grande não serão implementados.
- Animação baseada em física do corpo do jogador rolar no chão após o mesmo ser morto pelo Pé Grande não será implementada.

