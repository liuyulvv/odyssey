/**
 * @file odyssey.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_H_)
#define ODYSSEY_ODYSSEY_H_

#include <stdexcept>

#include "odyssey_pipeline.h"
#include "odyssey_window.h"

namespace odyssey {

class Odyssey {
public:
    Odyssey() = default;
    ~Odyssey() = default;
    Odyssey(const Odyssey& odyssey) = delete;
    Odyssey(Odyssey&& odyssey) = delete;
    Odyssey& operator=(const Odyssey& odyssey) = delete;
    Odyssey& operator=(Odyssey&& odyssey) = delete;

public:
    void Run();

private:
    void InitialVulkan();
    void MainLoop();
    void Cleanup();

public:
    static constexpr int WIDTH{800};
    static constexpr int HEIGHT{600};

private:
    OdysseyWindow window_{WIDTH, HEIGHT, "Odyssey"};
    OdysseyPipeline pipeline_{"shaders/vert_1.vert.spv", "shaders/frag_1.frag.spv"};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_H_