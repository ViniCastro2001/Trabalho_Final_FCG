#include "player.h"

#include <cmath>

#include "collisions.h"
#include "scene.h"

Player::Player(Camera* camera) : camera(camera) {}

void Player::Update(GLFWwindow* window, float delta_t)
{
    glm::vec4 view = camera->GetViewVector();

    // Projetamos o view no plano XZ para não voar ao olhar para cima/baixo
    glm::vec4 forward = glm::vec4(view.x, 0.0f, view.z, 0.0f);

    float forward_len = sqrt(forward.x*forward.x + forward.z*forward.z);

    if (forward_len > 0.0f)
        forward = forward / forward_len;

    // Vetor lateral da câmera.
    // Esse right aponta para a direita do jogador, considerando up = (0,1,0).
    glm::vec4 right = glm::vec4(-forward.z, 0.0f, forward.x, 0.0f);

    glm::vec4 movement = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movement += forward;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movement -= forward;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movement += right;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movement -= right;

    float movement_len = sqrt(movement.x*movement.x + movement.z*movement.z);

if (movement_len > 0.0f)
{
    movement = movement / movement_len;

    const float speed = 5.0f;
    const float player_radius = 0.35f;

    glm::vec4 current_position = camera->GetPosition();
    glm::vec4 desired_position = current_position + movement * (speed * delta_t);

    bool collided = false;

    const std::vector<BoxObstacle>& obstacles = GetSceneObstacles();

    for (const BoxObstacle& obstacle : obstacles)
    {
        if (CheckCircleBoxCollisionXZ(
                glm::vec3(desired_position.x, desired_position.y, desired_position.z),
                player_radius,
                obstacle
            ))
        {
            collided = true;
            break;
        }
    }

    if (!collided)
    {
        camera->Move(movement, speed * delta_t);
    }
}
}
