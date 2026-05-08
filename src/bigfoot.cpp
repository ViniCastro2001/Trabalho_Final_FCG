#include "bigfoot.h"

#include <cmath>
#include <vector>

#include "collisions.h"
#include "scene.h"
#include <cstdlib>

Bigfoot::Bigfoot()
{
    // Posição inicial do Pé Grande no cenário.
    // Por enquanto começamos ele mais ao fundo do mapa.
    position = glm::vec3(-6.0f, 1.0f, -6.5f);

    // Raio usado para colisão/detecção.
    radius = 0.8f;

    // Velocidade de perseguição.
    speed = 10.0f;

    // Distância em que consideramos que ele alcançou o player.
    attack_range = 1.2f;

    // Vida do Pé Grande.
    max_health = 100.0f;
    health = max_health;

    // Dados da fuga após levar tiro.
    flee_direction = glm::vec3(0.0f, 0.0f, 0.0f);
    flee_speed = 10.0f;
    flee_timer = 0.0f;
    flee_duration = 3.0f;

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

void Bigfoot::StartFleeing(glm::vec3 player_position)
{

    // Se já está fugindo, outro tiro não reinicia a fuga.
    if (state == BigfootState::Fleeing)
        return;

    // Fugimos na direção oposta ao player.
    flee_direction = position - player_position;
    flee_direction.y = 0.0f;

    float flee_direction_len = sqrt(
        flee_direction.x*flee_direction.x +
        flee_direction.z*flee_direction.z
    );

    if (flee_direction_len > 0.0f)
    {
        flee_direction = flee_direction / flee_direction_len;
    }
    else
    {
        // Caso extremo: se estiver exatamente na mesma posição do player.
        flee_direction = glm::vec3(0.0f, 0.0f, -1.0f);
    }

    // Adicionamos uma variação aleatória de -90 a +90 graus na direção da fuga.
    float random_value = (float)rand() / (float)RAND_MAX; // valor entre 0 e 1
    float angle_degrees = -90.0f + random_value * 180.0f;

    // Coverete de graus para radianos
    float angle = angle_degrees * 3.141592f / 180.0f;

    float old_x = flee_direction.x;
    float old_z = flee_direction.z;

    flee_direction.x = old_x * cos(angle) - old_z * sin(angle);
    flee_direction.z = old_x * sin(angle) + old_z * cos(angle);

    flee_timer = 0.0f;
    flee_duration = 3.0f;

    state = BigfootState::Fleeing;
}

void Bigfoot::TakeDamage(float damage, glm::vec3 player_position)
{
    if (IsDead())
        return;

    health -= damage;

    if (health < 0.0f)
        health = 0.0f;

    if (IsDead())
    {
        // Por enquanto, morrer só impede novas ações relevantes.
        return;
    }

    StartFleeing(player_position);
}

void Bigfoot::Update(glm::vec3 player_position, float delta_t)
{
    if (IsDead())
        return;

    if (state == BigfootState::Fleeing)
    {
        flee_timer += delta_t;

        float amount = flee_speed * delta_t;
        glm::vec3 desired_position = position + flee_direction * amount;

        // Primeiro tentamos o movimento completo da fuga.
        if (!CollidesWithScene(desired_position, radius))
        {
            position = desired_position;
        }
        else
        {
            // Se colidir, tentamos separar X e Z para permitir sliding simples.
            glm::vec3 movement_x = glm::vec3(flee_direction.x, 0.0f, 0.0f);
            glm::vec3 desired_position_x = position + movement_x * amount;

            if (!CollidesWithScene(desired_position_x, radius))
            {
                position = desired_position_x;
            }

            glm::vec3 position_after_x = position;

            glm::vec3 movement_z = glm::vec3(0.0f, 0.0f, flee_direction.z);
            glm::vec3 desired_position_z = position_after_x + movement_z * amount;

            if (!CollidesWithScene(desired_position_z, radius))
            {
                position = desired_position_z;
            }
        }

        if (flee_timer >= flee_duration)
        {
            state = BigfootState::Chasing;
        }

        return;
    }

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

float Bigfoot::GetHealth() const
{
    return health;
}

float Bigfoot::GetMaxHealth() const
{
    return max_health;
}

bool Bigfoot::IsDead() const
{
    return health <= 0.0f;
}