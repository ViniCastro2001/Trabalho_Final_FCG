#include "bigfoot.h"

#include <cmath>
#include <vector>

#include "collisions.h"
#include "scene.h"

Bigfoot::Bigfoot()
{
    // Posição inicial do Pé Grande no cenário.
    // Por enquanto começamos ele mais ao fundo do mapa.
    position = glm::vec3(-6.0f, 1.0f, -6.5f);

    // Raio usado para colisão/detecção.
    radius = 0.8f;

    // Velocidade de perseguição.
    speed = 2.0f;

    // Distância em que consideramos que ele alcançou o player.
    attack_range = 1.2f;

    state = BigfootState::Chasing;
}

static bool CollidesWithScene(glm::vec3 position, float radius)
{
    const std::vector<BoxObstacle>& obstacles = GetSceneObstacles();

    for (const BoxObstacle& obstacle : obstacles)
    {
        if (CheckCircleBoxCollisionXZ(position, radius, obstacle))
        {
            return true;
        }
    }

    return false;
}


void Bigfoot::Update(glm::vec3 player_position, float delta_t)
{
    if (state == BigfootState::Chasing)
    {
        glm::vec3 direction = player_position - position;

        // Ignoramos o eixo Y para o Pé Grande perseguir no chão.
        direction.y = 0.0f;

        float distance = sqrt(direction.x*direction.x + direction.z*direction.z);

        if (distance <= attack_range)
        {
            state = BigfootState::Attacking;
            return;
        }

        if (distance > 0.0f)
        {
            direction = direction / distance;

            float amount = speed * delta_t;
            glm::vec3 desired_position = position + direction * amount;

            // Primeiro tentamos o movimento direto até o player.
            if (!CollidesWithScene(desired_position, radius))
            {
                position = desired_position;
            }
            else
            {
                // Se colidir, tentamos separar X e Z para permitir sliding simples.
                glm::vec3 movement_x = glm::vec3(direction.x, 0.0f, 0.0f);
                glm::vec3 desired_position_x = position + movement_x * amount;

                if (!CollidesWithScene(desired_position_x, radius))
                {
                    position = desired_position_x;
                }

                glm::vec3 movement_z = glm::vec3(0.0f, 0.0f, direction.z);
                glm::vec3 desired_position_z = position + movement_z * amount;

                if (!CollidesWithScene(desired_position_z, radius))
                {
                    position = desired_position_z;
                }
            }
        }
    }
}

glm::vec3 Bigfoot::GetPosition() const
{
    return position;
}

float Bigfoot::GetRadius() const
{
    return radius;
}

BigfootState Bigfoot::GetState() const
{
    return state;
}