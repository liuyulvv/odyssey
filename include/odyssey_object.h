#pragma once

/**
 * @file odyssey_object.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-21
 */

#include <memory>

#include "odyssey_model.h"

namespace odyssey {

struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.0F, 1.0F, 1.0F};
    glm::vec3 rotation{};

    glm::mat4 mat4() {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        return glm::mat4{
            {
                scale.x * (c1 * c3 + s1 * s2 * s3),
                scale.x * (c2 * s3),
                scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                scale.y * (c3 * s1 * s2 - c1 * s3),
                scale.y * (c2 * c3),
                scale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                scale.z * (c2 * s1),
                scale.z * (-s2),
                scale.z * (c1 * c2),
                0.0f,
            },
            {translation.x, translation.y, translation.z, 1.0f}};
    }
};

class OdysseyObject {
private:
    explicit OdysseyObject(unsigned id);

public:
    OdysseyObject() = delete;
    ~OdysseyObject() = default;
    OdysseyObject(const OdysseyObject& odysseyObject) = delete;
    OdysseyObject(OdysseyObject&& odysseyObject) = default;
    OdysseyObject& operator=(const OdysseyObject& odysseyObject) = delete;
    OdysseyObject& operator=(OdysseyObject&& odysseyObject) = default;

    static OdysseyObject createObject() {
        static unsigned currentId = 0;
        return OdysseyObject(currentId++);
    }

public:
    unsigned getId() const;

public:
    std::shared_ptr<OdysseyModel> model{};
    glm::vec4 color{};
    TransformComponent transform{};

private:
    unsigned m_id;
};

}  // namespace odyssey