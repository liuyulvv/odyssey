#pragma once

/**
 * @file odyssey_camera.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-21
 */

#include "odyssey_header.h"

namespace odyssey {

class OdysseyCamera {
public:
    OdysseyCamera() = default;
    ~OdysseyCamera() = default;
    OdysseyCamera(const OdysseyCamera& odysseyCamera) = default;
    OdysseyCamera(OdysseyCamera&& odysseyCamera) = default;
    OdysseyCamera& operator=(const OdysseyCamera& odysseyCamera) = default;
    OdysseyCamera& operator=(OdysseyCamera&& odysseyCamera) = default;

public:
    void setOrthographicProjection(float left, float right, float top, float bottom, float nearValue, float farValue);
    void setPerspectiveProjection(float fovY, float aspect, float nearValue, float farValue);
    void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.0F, -1.0F, 0.0F});
    void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.0F, -1.0F, 0.0F});
    void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

    const glm::mat4& getProjection() const;
    const glm::mat4& getView() const;

private:
    glm::mat4 m_projectionMat{1.0F};
    glm::mat4 m_viewMat{1.0F};
};

}  // namespace odyssey