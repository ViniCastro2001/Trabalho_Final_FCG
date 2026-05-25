#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include "debug_flags.h"

#if MAP_VIEW_ENABLED

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>

#define MAP_MARKER_PLAYER 22
#define MAP_MARKER_BIGFOOT 23
#define MAP_MARKER_ITEM 24

class MapView
{
public:
    bool IsActive() const;
    void Toggle();

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float screen_ratio) const;

private:
    bool active = false;
};

#endif
#endif
