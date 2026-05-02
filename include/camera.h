#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec4.hpp>

// Classe para centralizar os dados da câmera.
// A ideia é evitar que o main.cpp fique responsável por guardar diretamente
// posição, direção e vetor up da câmera.

class Camera
{
public:
    Camera();

    // Getters usados pelo main.cpp para montar a matriz View com Matrix_Camera_View()
    glm::vec4 GetPosition() const;
    glm::vec4 GetViewVector() const;
    glm::vec4 GetUpVector() const;

    // Funções que serão usadas depois pelo mouse para alterar a direção da câmera
    void AddYaw(float delta);
    void AddPitch(float delta);

private:
    // Atualiza o vetor de visão com base nos ângulos yaw e pitch
    void UpdateViewVector();

    // position -> posição da câmera no mundo
    // view_vector -> direção para onde a câmera está olhando
    // up_vector -> vetor que indica o "cima" da câmera
    glm::vec4 position;
    glm::vec4 view_vector;
    glm::vec4 up_vector;

    // yaw -> rotação horizontal da câmera
    // pitch -> rotação vertical da câmera
    float yaw;
    float pitch;
};

#endif