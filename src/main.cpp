/**
 * @file main.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <QApplication>
#include <QGuiApplication>
#include <QVulkanInstance>

#include "odyssey_gui.h"
#include "odyssey_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QVulkanInstance instance;
#if !defined(NODEBUG)
    instance.setLayers({"VK_LAYER_KHRONOS_validation"});
#endif
    if (!instance.create()) 
        return -1;
    auto* odysseyWindow = new odyssey::OdysseyWindow();
    odysseyWindow->setVulkanInstance(&instance);
    odyssey::OdysseyGUI odysseyGUI(odysseyWindow);
    odysseyGUI.resize(1024, 768);
    odysseyGUI.show();
    return app.exec();
}