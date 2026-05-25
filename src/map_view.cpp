#include "map_view.h"

#if MAP_VIEW_ENABLED

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

bool MapView::IsActive() const
{
    return active;
}

void MapView::Toggle()
{
    active = !active;
}

glm::mat4 MapView::GetViewMatrix() const
{
    glm::vec3 eye = glm::vec3(0.0f, 120.0f, -51.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, -51.0f);
    glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);

    return glm::lookAt(eye, target, up);
}

glm::mat4 MapView::GetProjectionMatrix(float screen_ratio) const
{
    const float half_z = 92.0f;
    const float half_x = half_z * screen_ratio;

    return glm::ortho(-half_x, half_x, -half_z, half_z, 1.0f, 200.0f);
}

#endif
