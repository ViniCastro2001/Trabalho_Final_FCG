#include "player.h"

#include <cmath>
#include <vector>

#include "collisions.h"
#include "scene.h"

Player::Player(Camera* camera) : camera(camera) {}

static bool CollidesWithScene(glm::vec4 position, float player_radius)
{
    const std::vector<BoxObstacle>& obstacles = GetSceneObstacles();

    for (const BoxObstacle& obstacle : obstacles)
    {
        if (CheckCircleBoxCollisionXZ(
                glm::vec3(position.x, position.y, position.z),
                player_radius,
                obstacle
            ))
        {
            return true;
        }
    }

    return false;
}


static void CollectItems(glm::vec4 player_position)
{
    std::vector<Collectible>& collectibles = GetSceneCollectibles();

    for (Collectible& collectible : collectibles)
    {
        if (collectible.collected)
            continue;

        float dx = player_position.x - collectible.center.x;
        float dz = player_position.z - collectible.center.z;

        float distance_squared = dx*dx + dz*dz;

        if (distance_squared < collectible.radius * collectible.radius)
        {
            collectible.collected = true;
        }
    }
}


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

        float amount = speed * delta_t;

        glm::vec4 current_position = camera->GetPosition();
        glm::vec4 desired_position = current_position + movement * amount;

        // Primeiro tentamos o movimento completo.
        if (!CollidesWithScene(desired_position, player_radius))
        {
            camera->Move(movement, amount);
        }
        else
        {
            // Se o movimento completo colidiu, tentamos separar X e Z.
            // Isso permite deslizar ao longo das paredes/obstáculos.

            glm::vec4 movement_x = glm::vec4(movement.x, 0.0f, 0.0f, 0.0f);
            glm::vec4 desired_position_x = current_position + movement_x * amount;

            if (!CollidesWithScene(desired_position_x, player_radius))
            {
                camera->Move(movement_x, amount);
            }

            glm::vec4 position_after_x = camera->GetPosition();

            glm::vec4 movement_z = glm::vec4(0.0f, 0.0f, movement.z, 0.0f);
            glm::vec4 desired_position_z = position_after_x + movement_z * amount;

            if (!CollidesWithScene(desired_position_z, player_radius))
            {
                camera->Move(movement_z, amount);
            }
        }
    }


    CollectItems(camera->GetPosition());
}
