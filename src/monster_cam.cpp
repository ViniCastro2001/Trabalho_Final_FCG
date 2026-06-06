#include "monster_cam.h"

#include <cmath>

#include "matrices.h"

namespace
{
    float g_VisionSeconds = 0.0f;
    bool g_BigfootCamActive = false;

    // Rotaciona um vetor em torno do eixo Y (yaw). Mesma convenção do resto do
    // jogo: Z local aponta para frente.
    glm::vec3 RotateYaw(glm::vec3 v, float yaw)
    {
        float c = cos(yaw);
        float s = sin(yaw);

        return glm::vec3(
            c * v.x + s * v.z,
            v.y,
            -s * v.x + c * v.z
        );
    }

    glm::vec3 Normalize3(glm::vec3 v)
    {
        float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);

        if (len <= 0.0f)
            return glm::vec3(0.0f, 0.0f, 0.0f);

        return v / len;
    }
}

float GetVisionSeconds()
{
    return g_VisionSeconds;
}

void SetRawVisionSeconds(float seconds)
{
    if (seconds < 0.0f)
        seconds = 0.0f;

    g_VisionSeconds = seconds;
}

void AddVisionSeconds(float seconds)
{
    if (seconds <= 0.0f)
        return;

    g_VisionSeconds += seconds;
}

void ConsumeVisionSeconds(float seconds)
{
    if (seconds <= 0.0f)
        return;

    g_VisionSeconds -= seconds;

    if (g_VisionSeconds < 0.0f)
        g_VisionSeconds = 0.0f;
}

void ResetVisionSeconds()
{
    g_VisionSeconds = 0.0f;
}

bool IsBigfootCamActive()
{
    return g_BigfootCamActive;
}

void SetBigfootCamActive(bool active)
{
    g_BigfootCamActive = active;
}

bool UpdateBigfootCam(bool wants_to_activate, bool bigfoot_target_available, float delta_t)
{
    // A câmera só liga se o jogador a pediu, há um alvo e ainda restam segundos
    // de visão. Enquanto ativa, drena o recurso a cada frame — ao zerar, ela se
    // desliga sozinha no frame seguinte.
    g_BigfootCamActive =
        wants_to_activate &&
        bigfoot_target_available &&
        g_VisionSeconds > 0.0f;

    if (g_BigfootCamActive)
        ConsumeVisionSeconds(delta_t);

    return g_BigfootCamActive;
}

glm::mat4 ComputeBigfootCamView(glm::vec3 bigfoot_position, float bigfoot_yaw)
{
    // Cabeça em ~y=2.0 (ver hitbox em GetBigfootShotBoxes), levemente à frente
    // (+Z local) para a câmera ficar "nos olhos".
    glm::vec3 head_pos = bigfoot_position + RotateYaw(glm::vec3(0.0f, 2.1f, 0.45f), bigfoot_yaw);

    // Direção que o Pé Grande encara, com leve inclinação para baixo para
    // enxergar o chão logo à frente.
    glm::vec3 forward = RotateYaw(glm::vec3(0.0f, 0.0f, 1.0f), bigfoot_yaw);
    glm::vec3 look_dir = Normalize3(forward + glm::vec3(0.0f, -0.15f, 0.0f));

    return Matrix_Camera_View(
        glm::vec4(head_pos.x, head_pos.y, head_pos.z, 1.0f),
        glm::vec4(look_dir.x, look_dir.y, look_dir.z, 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)
    );
}
