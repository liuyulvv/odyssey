/**
 * @file odyssey_render.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_RENDER_H_)
#define ODYSSEY_RENDER_H_

#include <QFuture>
#include <QVulkanWindowRenderer>
#include <vector>

namespace odyssey {

class OdysseyWindow;

class OdysseyRender : public QVulkanWindowRenderer {
public:
    explicit OdysseyRender(OdysseyWindow* window);
    ~OdysseyRender() = default;

    OdysseyRender() = delete;
    OdysseyRender(const OdysseyRender& odysseyRender) = delete;
    OdysseyRender(OdysseyRender&& odysseyRender) = delete;
    OdysseyRender& operator=(const OdysseyRender& odysseyRender) = delete;
    OdysseyRender& operator=(OdysseyRender&& odysseyRender) = delete;

public:
    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;
    void startNextFrame() override;

private:
    OdysseyWindow* m_window{};
    QVulkanDeviceFunctions* m_deviceFuncs{};
    VkPipelineCache m_pipelineCache{VK_NULL_HANDLE};
    QFuture<void>
};

}  // namespace odyssey

#endif  // ODYSSEY_RENDER_H_