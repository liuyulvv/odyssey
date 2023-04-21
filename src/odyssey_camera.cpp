/**
 * @file odyssey_camera.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-21
 */

#include "odyssey_camera.h"

namespace odyssey {

void OdysseyCamera::setOrthographicProjection(float left, float right, float top, float bottom, float nearValue, float farValue) {
    m_projectionMat = glm::mat4{1.0F};
    m_projectionMat[0][0] = 2.0F / (right - left);
    m_projectionMat[1][1] = 2.0F / (bottom - top);
    m_projectionMat[2][2] = 1.0F / (farValue - nearValue);
    m_projectionMat[3][0] = -(right + left) / (right - left);
    m_projectionMat[3][1] = -(bottom + top) / (bottom - top);
    m_projectionMat[3][2] = -nearValue / (farValue - nearValue);
}

void OdysseyCamera::setPerspectiveProjection(float fovY, float aspect, float nearValue, float farValue) {
    const float tanHalfFovY = std::tanf(fovY / 2.0F);
    m_projectionMat = glm::mat4{0.0F};
    m_projectionMat[0][0] = 1.0F / (aspect * tanHalfFovY);
    m_projectionMat[1][1] = 1.0F / (tanHalfFovY);
    m_projectionMat[2][2] = farValue / (farValue - nearValue);
    m_projectionMat[2][3] = 1.0F;
    m_projectionMat[3][2] = -(farValue * nearValue) / (farValue - nearValue);
}

const glm::mat4& OdysseyCamera::getProjection() const {
    return m_projectionMat;
}

}  // namespace odyssey