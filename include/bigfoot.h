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

    glm::vec3 GetPosition() const;
    float GetRadius() const;
    BigfootState GetState() const;

private:
    glm::vec3 position;

    float radius;
    float speed;
    float attack_range;

    BigfootState state;
};

#endif