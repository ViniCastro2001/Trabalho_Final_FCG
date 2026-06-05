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

// Retorna a lista de blocos retangulares do cenário.
const std::vector<BoxObstacle>& GetSceneObstacles();

// Retorna a lista de itens coletáveis do cenário.
// Não é const porque precisamos marcar itens como coletados.
std::vector<Collectible>& GetSceneCollectibles();

// Retorna a zona segura/final do cenário.
// É const porque a posição e o tamanho da zona não mudam durante o jogo.
const SafeZone& GetSafeZone();

#endif