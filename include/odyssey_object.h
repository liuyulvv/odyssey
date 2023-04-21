#pragma once

/**
 * @file odyssey_object.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-21
 */

#include <memory>

#include "odyssey_model.h"

namespace odyssey {

struct Transform2DComponent {
    glm::vec2 translation{};
    glm::vec2 scale{1.F, 1.F};
    float rotation;

    glm::mat2 mat2() {
        const float s = glm::sin(rotation);
        const float c = glm::cos(rotation);
        glm::mat2 rotationMat{{c, s}, {-s, c}};
        glm::mat2 scaleMat{{scale.x, 0.F}, {0.F, scale.y}};
        return rotationMat * scaleMat;
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
    Transform2DComponent transform2D{};

private:
    unsigned m_id;
};

}  // namespace odyssey