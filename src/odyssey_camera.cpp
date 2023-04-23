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
    const float TAN_HALF_FOV_Y = std::tanf(fovY / 2.0F);
    m_projectionMat = glm::mat4{0.0F};
    m_projectionMat[0][0] = 1.0F / (aspect * TAN_HALF_FOV_Y);
    m_projectionMat[1][1] = 1.0F / (TAN_HALF_FOV_Y);
    m_projectionMat[2][2] = farValue / (farValue - nearValue);
    m_projectionMat[2][3] = 1.0F;
    m_projectionMat[3][2] = -(farValue * nearValue) / (farValue - nearValue);
}

void OdysseyCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    const glm::vec3 W{glm::normalize(direction)};
    const glm::vec3 U{glm::normalize(glm::cross(W, up))};
    const glm::vec3 V{glm::cross(W, U)};
    m_viewMat = glm::mat4{1.0F};
    m_viewMat[0][0] = U.x;
    m_viewMat[1][0] = U.y;
    m_viewMat[2][0] = U.z;
    m_viewMat[0][1] = V.x;
    m_viewMat[1][1] = V.y;
    m_viewMat[2][1] = V.z;
    m_viewMat[0][2] = W.x;
    m_viewMat[1][2] = W.y;
    m_viewMat[2][2] = W.z;
    m_viewMat[3][0] = -glm::dot(U, position);
    m_viewMat[3][1] = -glm::dot(V, position);
    m_viewMat[3][2] = -glm::dot(W, position);
}

void OdysseyCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    setViewDirection(position, target - position, up);
}

void OdysseyCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
    const float C3 = glm::cos(rotation.z);
    const float S3 = glm::sin(rotation.z);
    const float C2 = glm::cos(rotation.x);
    const float S2 = glm::sin(rotation.x);
    const float C1 = glm::cos(rotation.y);
    const float S1 = glm::sin(rotation.y);
    const glm::vec3 U{(C1 * C3 + S1 * S2 * S3), (C2 * S3), (C1 * S2 * S3 - C3 * S1)};
    const glm::vec3 V{(C3 * S1 * S2 - C1 * S3), (C2 * C3), (C1 * C3 * S2 + S1 * S3)};
    const glm::vec3 W{(C2 * S1), (-S2), (C1 * C2)};
    m_viewMat = glm::mat4{1.0F};
    m_viewMat[0][0] = U.x;
    m_viewMat[1][0] = U.y;
    m_viewMat[2][0] = U.z;
    m_viewMat[0][1] = V.x;
    m_viewMat[1][1] = V.y;
    m_viewMat[2][1] = V.z;
    m_viewMat[0][2] = W.x;
    m_viewMat[1][2] = W.y;
    m_viewMat[2][2] = W.z;
    m_viewMat[3][0] = -glm::dot(U, position);
    m_viewMat[3][1] = -glm::dot(V, position);
    m_viewMat[3][2] = -glm::dot(W, position);
}

const glm::mat4& OdysseyCamera::getProjection() const {
    return m_projectionMat;
}

const glm::mat4& OdysseyCamera::getView() const {
    return m_viewMat;
}

}  // namespace odyssey