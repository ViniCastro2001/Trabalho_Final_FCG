#include "collisions.h"

#include <algorithm>

bool CheckCircleBoxCollisionXZ(
    glm::vec3 circle_center,
    float circle_radius,
    BoxObstacle box
)
{
    // Como o BoxObstacle guarda center/size, calculamos os limites do bloco.
    // Por enquanto, a colisão é só no plano XZ. Ignoramos Y.
    float box_min_x = box.center.x - box.size.x / 2.0f; // Borda esquerda
    float box_max_x = box.center.x + box.size.x / 2.0f; // Borda direita

    float box_min_z = box.center.z - box.size.z / 2.0f; // Borda mínima em Z
    float box_max_z = box.center.z + box.size.z / 2.0f; // Borda máxima em Z

    // Ponto do bloco mais próximo do centro do círculo.
    float closest_x = std::max(box_min_x, std::min(circle_center.x, box_max_x));
    float closest_z = std::max(box_min_z, std::min(circle_center.z, box_max_z));

    // Distância entre o centro do círculo e esse ponto mais próximo.
    float dx = circle_center.x - closest_x;
    float dz = circle_center.z - closest_z;

    // Comparando distância² com raio² para não precisar usar sqrt().
    float distance_squared = dx*dx + dz*dz;

    return distance_squared < circle_radius * circle_radius;
}