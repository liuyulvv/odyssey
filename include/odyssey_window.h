/**
 * @file odyssey_window.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_WINDOW_H_)
#define ODYSSEY_WINDOW_H_

#include <vulkan/vulkan.h>

#include <QVulkanInstance>
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

namespace odyssey {

class OdysseyWindow : public QVulkanWindow {
public:
    OdysseyWindow();
    ~OdysseyWindow() = default;
    OdysseyWindow(const OdysseyWindow& odysseyWindow) = delete;
    OdysseyWindow(OdysseyWindow&& odysseyWindow) = delete;
    OdysseyWindow& operator=(const OdysseyWindow& odysseyWindow) = delete;
    OdysseyWindow& operator=(OdysseyWindow&& odysseyWindow) = delete;

public:
    QVulkanWindowRenderer* createRenderer() override;

private:
    QVulkanInstance* m_instance{nullptr};
};

}  // namespace odyssey

#endif  // ODYSSEY_WINDOW_H_