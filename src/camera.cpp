#include "camera.h"

#include <cmath>

static float NormalizeAngle(float angle)
{
    const float pi = 3.141592f;

    while (angle > pi)
        angle -= 2.0f * pi;

    while (angle < -pi)
        angle += 2.0f * pi;

    return angle;
}

Camera::Camera()
{
    // x -> Esquerda/Direita, não tem porque alterar inicialmente
    // y -> Altura inicial da câmera/jogador. A não ser que venhamos a implementar pulo, subidas ou descidas, não tem porque mudar
    // z -> usamos 3.5f por ser a distância inicial da câmera no template
    // w -> câmera possui posição no mundo -> w = 1.0f
    position = glm::vec4(0.0f, 1.7f, 27.0f, 1.0f);

    // up_vector -> aponta para o "céu" da cena.
    // Como o eixo Y é o eixo vertical do mundo, usamos (0, 1, 0).
    // w = 0.0f porque é um vetor, não uma posição.
    up_vector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

    // yaw e pitch definem para onde a câmera olha.
    // yaw = -pi/2 faz a câmera começar olhando para -Z.
    // pitch = 0.0f significa olhar reto, sem inclinar para cima ou para baixo.
    yaw = -3.141592f / 2.0f;
    pitch = 0.0f;

    // Calcula o view_vector inicial a partir dos ângulos definidos acima.
    UpdateViewVector();
}

// Implementando getters

glm::vec4 Camera::GetPosition() const
{
    return position;
}

glm::vec4 Camera::GetViewVector() const
{
    return view_vector;
}

glm::vec4 Camera::GetUpVector() const
{
    return up_vector;
}

// Altera a rotação horizontal da câmera.
// Depois de alterar o ângulo, recalcula o vetor de visão.
void Camera::AddYaw(float delta)
{
    yaw += delta;

    UpdateViewVector();
}

// Altera a rotação vertical da câmera.
// O pitch precisa ser limitado para impedir que a câmera vire de cabeça para baixo.
void Camera::AddPitch(float delta)
{
    pitch += delta;

    // 1.55 radianos é quase 90 graus.
    // Isso permite olhar quase totalmente para cima/baixo, mas sem inverter a câmera.
    const float limit = 1.55f;

    if (pitch > limit)
        pitch = limit;

    if (pitch < -limit)
        pitch = -limit;

    UpdateViewVector();
}

void Camera::LookAt(glm::vec3 target, float max_angle_step)
{
    glm::vec3 to_target = target - glm::vec3(position.x, position.y, position.z);
    float horizontal_len = sqrt(to_target.x*to_target.x + to_target.z*to_target.z);

    if (horizontal_len <= 0.0001f)
        return;

    float target_yaw = atan2(to_target.z, to_target.x);
    float target_pitch = atan2(to_target.y, horizontal_len);
    float yaw_delta = NormalizeAngle(target_yaw - yaw);
    float pitch_delta = target_pitch - pitch;

    if (max_angle_step > 0.0f)
    {
        if (yaw_delta > max_angle_step) yaw_delta = max_angle_step;
        if (yaw_delta < -max_angle_step) yaw_delta = -max_angle_step;
        if (pitch_delta > max_angle_step) pitch_delta = max_angle_step;
        if (pitch_delta < -max_angle_step) pitch_delta = -max_angle_step;
    }

    yaw += yaw_delta;
    pitch += pitch_delta;

    const float limit = 1.55f;

    if (pitch > limit)
        pitch = limit;

    if (pitch < -limit)
        pitch = -limit;

    UpdateViewVector();
}

void Camera::Move(glm::vec4 direction, float amount)
{
    position += direction * amount;

    // Garantimos que position continua sendo um ponto em coordenadas homogêneas.
    position.w = 1.0f;
}

// Converte yaw/pitch para um vetor de direção em 3D.
// Esse vetor será usado depois pela Matrix_Camera_View().
void Camera::UpdateViewVector()
{
    float x = cos(pitch) * cos(yaw);
    float y = sin(pitch);
    float z = cos(pitch) * sin(yaw);

    // w = 0.0f porque isso é uma direção, não uma posição.
    view_vector = glm::vec4(x, y, z, 0.0f);
}
