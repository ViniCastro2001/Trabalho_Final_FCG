#include "bigfoot.h"

#include <cmath>

Bigfoot::Bigfoot()
{
    // Posição inicial do Pé Grande no cenário.
    // Por enquanto começamos ele mais ao fundo do mapa.
    position = glm::vec3(0.0f, 1.0f, -8.0f);

    // Raio usado para colisão/detecção.
    radius = 0.8f;

    // Velocidade de perseguição.
    speed = 2.0f;

    // Distância em que consideramos que ele alcançou o player.
    attack_range = 1.2f;

    state = BigfootState::Chasing;
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
            position += direction * speed * delta_t;
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