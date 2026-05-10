#include "bigfoot.h"

#include <cmath>
#include <vector>
#include <cstdlib>

#include "collisions.h"
#include "scene.h"

static glm::vec3 Lerp(glm::vec3 a, glm::vec3 b, float t)
{
    return a + t * (b - a);
}

Bigfoot::Bigfoot()
{
    // Posição inicial do Pé Grande no cenário.
    // Por enquanto começamos ele mais ao fundo do mapa.
    position = glm::vec3(-6.0f, 1.0f, -18.0f);

    // Raio usado para colisão/detecção.
    radius = 0.8f;

    // Velocidade de perseguição.
    speed = 15.0f;

    // Distância em que consideramos que ele alcançou o player.
    attack_range = 1.2f;

    // Vida do Pé Grande.
    max_health = 100.0f;
    health = max_health;

    // Dados da fuga após levar tiro.
    flee_direction = glm::vec3(0.0f, 0.0f, 0.0f);
    flee_speed = 16.0f;
    flee_timer = 0.0f;
    flee_duration = 3.0f;


    // Pontos de controle da curva Bézier usada durante a fuga.
    bezier_p0 = glm::vec3(0.0f, 0.0f, 0.0f);
    bezier_p1 = glm::vec3(0.0f, 0.0f, 0.0f);
    bezier_p2 = glm::vec3(0.0f, 0.0f, 0.0f);
    bezier_p3 = glm::vec3(0.0f, 0.0f, 0.0f);

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

static void MoveWithCollisionSliding(glm::vec3& position, glm::vec3 movement, float radius)
{
    float distance = sqrt(movement.x*movement.x + movement.z*movement.z);

    // Quebra movimentos grandes em passos menores para evitar atravessar paredes.
    int steps = (int)(distance / 0.20f) + 1;

    for (int i = 0; i < steps; ++i)
    {
        glm::vec3 step_movement = movement / (float)steps;

        glm::vec3 desired_position = position + step_movement;

        // Primeiro tenta o movimento completo do subpasso.
        if (!CollidesWithScene(desired_position, radius))
        {
            position = desired_position;
        }
        else
        {
            // Se colidir, tenta separar X e Z para permitir sliding simples.
            glm::vec3 desired_position_x = position + glm::vec3(step_movement.x, 0.0f, 0.0f);

            if (!CollidesWithScene(desired_position_x, radius))
            {
                position = desired_position_x;
            }

            glm::vec3 desired_position_z = position + glm::vec3(0.0f, 0.0f, step_movement.z);

            if (!CollidesWithScene(desired_position_z, radius))
            {
                position = desired_position_z;
            }
        }
    }
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

    // Reseta o timer e define a duração da fuga.
    flee_timer = 0.0f;

    // Duração aleatória da fuga entre 5 e 15 segundos.
    float duration_random = (float)rand() / (float)RAND_MAX;
    flee_duration = 3.0f + duration_random * 4.0f;

    // Definimos a curva Bézier cúbica usada no recuo.
    // P0 é a posição atual do Pé Grande.
    bezier_p0 = position;

    // Usamos flee_speed para calcular a distância total da fuga.
    float flee_distance = flee_speed * flee_duration;

    // Evita que o Pé Grande fuja longe demais em fugas longas.
    if (flee_distance > 50.0f)
        flee_distance = 50.0f;


    // P3 é o ponto final da fuga, na direção calculada.
    bezier_p3 = bezier_p0 + flee_direction * flee_distance;


    // Direção lateral perpendicular à direção da fuga no plano XZ.
    glm::vec3 side_direction = glm::vec3(-flee_direction.z, 0.0f, flee_direction.x);

    // Deslocamento lateral aleatório para tornar a curva menos previsível.
    float side_random = (float)rand() / (float)RAND_MAX;
    float side_offset = -20.0f + side_random * 40.0f;

    // P1 e P2 puxam a trajetória para os lados, formando uma curva.
    bezier_p1 = bezier_p0
            + flee_direction * (flee_distance * 0.33f)
            + side_direction * side_offset;

    bezier_p2 = bezier_p0
            + flee_direction * (flee_distance * 0.66f)
            - side_direction * side_offset;

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

glm::vec3 Bigfoot::ComputeBezierPoint(float t) const
{
    // Garantimos que t fique no intervalo [0, 1].
    if (t < 0.0f)
        t = 0.0f;

    if (t > 1.0f)
        t = 1.0f;

    // Algoritmo de De Casteljau para Bézier cúbica.
    // 4 pontos -> 3 pontos.
    glm::vec3 p01 = Lerp(bezier_p0, bezier_p1, t);
    glm::vec3 p12 = Lerp(bezier_p1, bezier_p2, t);
    glm::vec3 p23 = Lerp(bezier_p2, bezier_p3, t);

    // 3 pontos -> 2 pontos.
    glm::vec3 p012 = Lerp(p01, p12, t);
    glm::vec3 p123 = Lerp(p12, p23, t);

    // 2 pontos -> 1 ponto final da curva.
    glm::vec3 point = Lerp(p012, p123, t);

    return point;
}

void Bigfoot::Update(glm::vec3 player_position, float delta_t)
{
    if (IsDead())
        return;

    if (state == BigfootState::Fleeing)
    {
        flee_timer += delta_t;

        float t = flee_timer / flee_duration;

        if (t > 1.0f)
            t = 1.0f;

        glm::vec3 desired_position = ComputeBezierPoint(t);

        // Movimento desejado na direção do ponto atual da curva.
        glm::vec3 movement = desired_position - position;

        // Limitamos o tamanho do movimento por frame para evitar teleportes
        // quando o Pé Grande fica preso em uma parede e a curva continua avançando.
        float movement_length = sqrt(movement.x*movement.x + movement.z*movement.z);

        float max_step = flee_speed * delta_t;

        if (movement_length > max_step && movement_length > 0.0f)
        {
            movement = movement / movement_length;
            movement = movement * max_step;
        }

        MoveWithCollisionSliding(position, movement, radius);

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
            glm::vec3 movement = direction * amount;

            MoveWithCollisionSliding(position, movement, radius);
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