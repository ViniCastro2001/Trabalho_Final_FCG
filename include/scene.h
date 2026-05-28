#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <glm/vec3.hpp>

// Representa um bloco retangular do cenário.
// O mesmo center/size será usado para desenhar o cubo e depois para colisão.
struct BoxObstacle
{
    glm::vec3 center;
    glm::vec3 size;
};

// Representa um item coletável do cenário.
// O player coleta quando chega perto o suficiente do center.
struct Collectible
{
    glm::vec3 center;
    float radius;
    bool collected;
};

// Representa a área segura/final do jogo.
// Se o player coletar todos os itens e entrar nela, vence.
struct SafeZone
{
    glm::vec3 center;
    glm::vec3 size;
};

// Poste de luz: emite luz pontual com atenuação. Tudo no jogo é iluminado
// pelos postes; a lâmpada fica em "base + bulb_offset".
struct LightPost
{
    glm::vec3 base;        // posição da base do poste no chão
    glm::vec3 bulb_offset; // deslocamento até a lâmpada (geralmente (0, h, 0))
    glm::vec3 color;       // cor RGB da luz
    float intensity;       // multiplicador de brilho
    float range;           // distância máxima de iluminação
};

// Retorna a lista de blocos retangulares do cenário.
const std::vector<BoxObstacle>& GetSceneObstacles();

// Retorna a lista de itens coletáveis do cenário.
// Não é const porque precisamos marcar itens como coletados.
std::vector<Collectible>& GetSceneCollectibles();

// Retorna a zona segura/final do cenário.
// É const porque a posição e o tamanho da zona não mudam durante o jogo.
const SafeZone& GetSafeZone();

// Retorna os postes de luz espalhados pelo mapa.
const std::vector<LightPost>& GetSceneLightPosts();

#endif