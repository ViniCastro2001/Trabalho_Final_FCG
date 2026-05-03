#ifndef PLAYER_H
#define PLAYER_H

#include <GLFW/glfw3.h>
#include "camera.h"

class Player
{
public:
    Player(Camera* camera);

    // Lê o teclado e move a câmera. Deve ser chamado uma vez por frame.
    void Update(GLFWwindow* window, float delta_t);

private:
    Camera* camera;
};

#endif
