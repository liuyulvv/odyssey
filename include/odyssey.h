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
#include "odyssey_pipeline.h"
#include "odyssey_swap_chain.h"
#include "odyssey_window.h"
#include "vulkan/vulkan.hpp"

namespace odyssey {

class Odyssey {
public:
    Odyssey();
    ~Odyssey();
    Odyssey(const Odyssey& odyssey) = delete;
    Odyssey(Odyssey&& odyssey) = delete;
    Odyssey& operator=(const Odyssey& odyssey) = delete;
    Odyssey& operator=(Odyssey&& odyssey) = delete;

public:
    void Run();

private:
    void MainLoop();
    void CreatePipelineLayout();
    void CreatePipeline();
    void CreateCommandBuffers();
    void Draw();

public:
    static constexpr int WIDTH{800};
    static constexpr int HEIGHT{600};

private:
    OdysseyWindow window_{WIDTH, HEIGHT, "Odyssey"};
    OdysseyEngine engine_{window_};
    OdysseySwapChain swap_chain_{engine_, window_.GetExtent()};
    std::unique_ptr<OdysseyPipeline> pipeline_{};
    vk::PipelineLayout pipeline_layout_{};
    std::vector<vk::CommandBuffer> command_buffers_{};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_H_