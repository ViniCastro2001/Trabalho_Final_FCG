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

// Retorna a lista de blocos retangulares do cenário.
const std::vector<BoxObstacle>& GetSceneObstacles();

// Retorna a lista de itens coletáveis do cenário.
// Não é const porque precisamos marcar itens como coletados.
std::vector<Collectible>& GetSceneCollectibles();

#endif