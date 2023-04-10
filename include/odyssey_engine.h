/**
 * @file odyssey_engine.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_ENGINE_H_)
#define ODYSSEY_ODYSSEY_ENGINE_H_

#include "odyssey_window.h"
#include "vulkan/vulkan.hpp"

namespace odyssey {

class OdysseyEngine {
public:
    explicit OdysseyEngine(OdysseyWindow& window);
    ~OdysseyEngine() = default;

    OdysseyEngine() = delete;
    OdysseyEngine(const OdysseyEngine& odyssey_engine) = delete;
    OdysseyEngine(OdysseyEngine&& odyssey_engine) = delete;
    OdysseyEngine& operator=(const OdysseyEngine& odyssey_engine) = delete;
    OdysseyEngine& operator=(OdysseyEngine&& odyssey_engine) = delete;

private:
    void CreateInstance();

private:
    bool CheckValidationLayerSupport();
    void CheckExtensionsSupport();
    std::vector<const char*> GetRequiredExtensions() const;

private:
    OdysseyWindow& window_;
#if defined(NODEBUG)
    bool enable_validation_layers_ = false;
#else
    bool enable_validation_layers_ = true;
    std::vector<const char*> validation_layers{"VK_LAYER_KHRONOS_validation"};
#endif  // NODEBUG
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_ENGINE_H_