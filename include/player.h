#ifndef PLAYER_H
#define PLAYER_H

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include "camera.h"

class Player
{
public:
    Player(Camera* camera);

    // Lê o teclado e move a câmera. Deve ser chamado uma vez por frame.
    void Update(GLFWwindow* window, float delta_t);
    void UpdateAutonomous(glm::vec3 movement_direction, bool running, float delta_t);
    bool IsEnergyBoostActive() const;
    float GetEnergyBoostTimeRemaining() const;
    void ResetEnergyBoost();
    void SetInfiniteBoostCheat(bool enabled);
    bool IsInfiniteBoostCheatActive() const;

private:
    void TickEnergyBoost(float delta_t);
    void MoveAndCollect(glm::vec3 movement_direction, bool running, float delta_t);

    Camera* camera;
    float energy_boost_timer;
    bool infinite_boost_cheat;
};

#endif
