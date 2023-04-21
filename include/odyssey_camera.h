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

    const glm::mat4& getProjection() const;

private:
    glm::mat4 m_projectionMat{1.0F};
};

}  // namespace odyssey