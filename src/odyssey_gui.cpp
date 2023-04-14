/**
 * @file odyssey_gui.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-12
 */

#include "odyssey_gui.h"

#include <QVBoxLayout>

#include "odyssey_window.h"

namespace odyssey {

OdysseyGUI::OdysseyGUI(OdysseyWindow* window) {
    auto* wrapper = QWidget::createWindowContainer(window);
    auto* layout = new QVBoxLayout();
    layout->addWidget(wrapper);
    setLayout(layout);
}

}  // namespace odyssey