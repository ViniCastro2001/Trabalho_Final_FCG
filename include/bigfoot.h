#ifndef BIGFOOT_H
#define BIGFOOT_H

#include <glm/vec3.hpp>

enum class BigfootState
{
    Chasing,
    Attacking,
    Fleeing
};

class Bigfoot
{
public:
    Bigfoot();

    // Atualiza o Pé Grande a cada frame.
    // Por enquanto, ele apenas persegue o player e entra em ataque se chegar perto.
    void Update(glm::vec3 player_position, float delta_t);

    // Faz o Pé Grande fugir para longe do player após levar um tiro.
    void StartFleeing(glm::vec3 player_position);

    // Aplica dano ao Pé Grande.
    void TakeDamage(float damage, glm::vec3 player_position);
    void ApplyDifficultyMultipliers(float speed_multiplier, float health_multiplier);

    glm::vec3 GetPosition() const;
    float GetRadius() const;
    BigfootState GetState() const;
    float GetFacingYaw() const;

    float GetHealth() const;
    float GetMaxHealth() const;
    bool IsDead() const;

private:
    glm::vec3 position;

    float radius;
    float speed;
    float attack_range;

    float health;
    float max_health;
    glm::vec3 facing_direction;

    // Dados da fuga após levar tiro.
    glm::vec3 flee_direction;
    float flee_speed;
    float flee_timer;
    float flee_duration;

    // Pontos de controle da curva Bézier usada durante a fuga.
    glm::vec3 bezier_p0;
    glm::vec3 bezier_p1;
    glm::vec3 bezier_p2;
    glm::vec3 bezier_p3;

    glm::vec3 ComputeBezierPoint(float t) const;
    void UpdateFacingFromMovement(glm::vec3 start_position);

    BigfootState state;
};

#endif
