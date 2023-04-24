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

    glm::mat4 mat4();
    glm::mat3 normal();
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