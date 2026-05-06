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

// Retorna a lista de blocos retangulares do cenário.
const std::vector<BoxObstacle>& GetSceneObstacles();

#endif