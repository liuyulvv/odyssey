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
#include "odyssey_gui.h"
#include "odyssey_model.h"
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
    std::unique_ptr<OdysseyPipeline> CreatePipeline(const std::string& vert_shader_path, const std::string& frag_shader_path, vk::PrimitiveTopology primitive_topology, float line_width);
    void CreateCommandBuffers();
    void Draw();
    void LoadModel();
    void RecreateSwapChain();
    void RecordCommandBuffer(uint32_t image_index);

public:
    static constexpr int WIDTH{800};
    static constexpr int HEIGHT{600};

private:
    std::shared_ptr<OdysseyWindow> window_{};
    std::shared_ptr<OdysseyEngine> engine_{};
    std::unique_ptr<OdysseySwapChain> swap_chain_{};
    std::unique_ptr<OdysseyPipeline> pipeline_line_{};
    vk::PipelineLayout pipeline_layout_{};
    std::vector<vk::CommandBuffer> command_buffers_{};
    std::unique_ptr<OdysseyModel> model_{};
    std::unique_ptr<OdysseyGUI> gui_{};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_H_