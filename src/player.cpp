#include "player.h"

#include <cmath>

Player::Player(Camera* camera) : camera(camera) {}

void Player::Update(GLFWwindow* window, float delta_t)
{
    glm::vec4 view = camera->GetViewVector();
    glm::vec4 up   = camera->GetUpVector();

    // Projetamos o view no plano XZ para não voar ao olhar para cima/baixo
    glm::vec4 forward = glm::vec4(view.x, 0.0f, view.z, 0.0f);
    float len = sqrt(forward.x*forward.x + forward.z*forward.z);
    if (len > 0.0f) forward = forward / len;

    // Produto vetorial forward x up simplificado para up = (0,1,0)
    glm::vec4 right = glm::vec4(forward.z, 0.0f, -forward.x, 0.0f);

    const float speed = 5.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->Move(forward,  speed * delta_t);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->Move(forward, -speed * delta_t);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->Move(right,   -speed * delta_t);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->Move(right,    speed * delta_t);
}
