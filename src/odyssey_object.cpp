#pragma once

/**
 * @file odyssey_object.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-21
 */

#include "odyssey_object.h"

namespace odyssey {

glm::mat4 TransformComponent::mat4() {
    const float C3 = glm::cos(rotation.z);
    const float S3 = glm::sin(rotation.z);
    const float C2 = glm::cos(rotation.x);
    const float S2 = glm::sin(rotation.x);
    const float C1 = glm::cos(rotation.y);
    const float S1 = glm::sin(rotation.y);
    return glm::mat4{
        {
            scale.x * (C1 * C3 + S1 * S2 * S3),
            scale.x * (C2 * S3),
            scale.x * (C1 * S2 * S3 - C3 * S1),
            0.0F,
        },
        {
            scale.y * (C3 * S1 * S2 - C1 * S3),
            scale.y * (C2 * C3),
            scale.y * (C1 * C3 * S2 + S1 * S3),
            0.0F,
        },
        {
            scale.z * (C2 * S1),
            scale.z * (-S2),
            scale.z * (C1 * C2),
            0.0F,
        },
        {translation.x, translation.y, translation.z, 1.0F}};
}

glm::mat3 TransformComponent::normal() {
    const float C3 = glm::cos(rotation.z);
    const float S3 = glm::sin(rotation.z);
    const float C2 = glm::cos(rotation.x);
    const float S2 = glm::sin(rotation.x);
    const float C1 = glm::cos(rotation.y);
    const float S1 = glm::sin(rotation.y);
    glm::vec3 inverseScale = 1.0F / scale;
    return glm::mat3{
        {
            inverseScale.x * (C1 * C3 + S1 * S2 * S3),
            inverseScale.x * (C2 * S3),
            inverseScale.x * (C1 * S2 * S3 - C3 * S1),
        },
        {
            inverseScale.y * (C3 * S1 * S2 - C1 * S3),
            inverseScale.y * (C2 * C3),
            inverseScale.y * (C1 * C3 * S2 + S1 * S3),
        },
        {
            inverseScale.z * (C2 * S1),
            inverseScale.z * (-S2),
            inverseScale.z * (C1 * C2),
        },
    };
}

OdysseyObject::OdysseyObject(unsigned id) : m_id(id) {
}

}  // namespace odyssey