#include "player.h"

#include <cmath>
#include <vector>

#include "audio.h"
#include "collisions.h"
#include "scene.h"
#include "upgrades.h"

extern void SavePrestigeMemory();

Player::Player(Camera* camera)
    : camera(camera), energy_boost_timer(0.0f), infinite_boost_cheat(false) {}

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

static int CollectItems(glm::vec4 player_position)
{
    std::vector<Collectible>& collectibles = GetSceneCollectibles();
    int collected_count = 0;

    float extra_radius = GetUpgradeValue(UpgradeId::CollectRadius);

    for (Collectible& collectible : collectibles)
    {
        if (collectible.collected)
            continue;

        float dx = player_position.x - collectible.center.x;
        float dz = player_position.z - collectible.center.z;

        float distance_squared = dx*dx + dz*dz;
        float effective_radius = collectible.radius + extra_radius;

        if (distance_squared < effective_radius * effective_radius)
        {
            collectible.collected = true;
            collected_count++;
            PlayGameSound(GameSound::CollectDrink);
        }
    }

    return collected_count;
}

void Player::TickEnergyBoost(float delta_t)
{
    if (energy_boost_timer > 0.0f)
    {
        energy_boost_timer -= delta_t;

        if (energy_boost_timer < 0.0f)
            energy_boost_timer = 0.0f;
    }
}

void Player::Update(GLFWwindow* window, float delta_t)
{
    glm::vec4 view = camera->GetViewVector();

    // Projetamos o view no plano XZ para nao voar ao olhar para cima/baixo.
    glm::vec4 forward = glm::vec4(view.x, 0.0f, view.z, 0.0f);

    float forward_len = sqrt(forward.x*forward.x + forward.z*forward.z);

    if (forward_len > 0.0f)
        forward = forward / forward_len;

    // Vetor lateral da camera, apontando para a direita do jogador.
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

    bool running =
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

    MoveAndCollect(glm::vec3(movement.x, 0.0f, movement.z), running, delta_t);
}

void Player::UpdateAutonomous(glm::vec3 movement_direction, bool running, float delta_t)
{
    MoveAndCollect(movement_direction, running, delta_t);
}

void Player::MoveAndCollect(glm::vec3 movement_direction, bool running, float delta_t)
{
    TickEnergyBoost(delta_t);

    glm::vec4 movement = glm::vec4(movement_direction.x, 0.0f, movement_direction.z, 0.0f);
    float movement_len = sqrt(movement.x*movement.x + movement.z*movement.z);

    if (movement_len > 0.0f)
    {
        movement = movement / movement_len;

        static float footstep_timer = 0.0f;

        footstep_timer -= delta_t;

        bool boosted = IsEnergyBoostActive();

        if (footstep_timer <= 0.0f)
        {
            PlayGameSound(GameSound::Footstep);
            footstep_timer = (running ? 0.25f : 0.42f) * (boosted ? 0.5f : 1.0f);
        }

        float move_speed_mult = GetUpgradeValue(UpgradeId::MoveSpeed);
        float boost_mult      = GetUpgradeValue(UpgradeId::BoostMultiplier);

        float speed = (running ? 9.2f : 5.8f) * move_speed_mult;

        if (boosted)
            speed *= boost_mult;

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
            // Isso permite deslizar ao longo das paredes/obstaculos.
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

    int collected_count = CollectItems(camera->GetPosition());

    if (collected_count > 0)
    {
        float boost_per_item = GetUpgradeValue(UpgradeId::BoostDuration);
        energy_boost_timer += boost_per_item * collected_count;
        AddCoins(collected_count);
        SavePrestigeMemory();
    }
}

bool Player::IsEnergyBoostActive() const
{
    return infinite_boost_cheat || energy_boost_timer > 0.0f;
}

float Player::GetEnergyBoostTimeRemaining() const
{
    return energy_boost_timer;
}

void Player::ResetEnergyBoost()
{
    energy_boost_timer = 0.0f;
}

void Player::SetInfiniteBoostCheat(bool enabled)
{
    infinite_boost_cheat = enabled;
}

bool Player::IsInfiniteBoostCheatActive() const
{
    return infinite_boost_cheat;
}
