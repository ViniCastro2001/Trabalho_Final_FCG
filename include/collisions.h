#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <glm/vec3.hpp>

#include "scene.h"

// Testa colisão entre um círculo no plano XZ e um bloco retangular.
// Player/Pé Grande serão tratados como círculo visto de cima.
// Blocos do cenário serão BoxObstacle.
bool CheckCircleBoxCollisionXZ(
    glm::vec3 circle_center,
    float circle_radius,
    BoxObstacle box
);

#endif