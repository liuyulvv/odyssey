/**
 * @file odyssey.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_H_)
#define ODYSSEY_ODYSSEY_H_

#include <memory>
#include <vector>

#include "odyssey_engine.h"
#include "odyssey_model.h"
#include "odyssey_pipeline.h"
#include "odyssey_swap_chain.h"
#include "odyssey_window.h"

#if defined(_WIN32)
#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vulkan.hpp"

namespace odyssey {

class Odyssey {
public:
    Odyssey();
    ~Odyssey() = default;
    Odyssey(const Odyssey& odyssey) = delete;
    Odyssey(Odyssey&& odyssey) = delete;
    Odyssey& operator=(const Odyssey& odyssey) = delete;
    Odyssey& operator=(Odyssey&& odyssey) = delete;

public:
    void Run();

private:
    void Draw();

public:
    static constexpr int WIDTH{800};
    static constexpr int HEIGHT{600};

private:
    std::shared_ptr<OdysseyWindow> window_{};
    std::shared_ptr<OdysseyEngine> engine_{};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_H_