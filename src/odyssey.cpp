/**
 * @file odyssey.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-12
 */

#include "odyssey.h"

#include <QVBoxLayout>

#include "odyssey_window.h"

namespace odyssey {

Odyssey::Odyssey() : m_window(new OdysseyWindow()) {
    auto* wrapper = QWidget::createWindowContainer(m_window);
    auto* layout = new QVBoxLayout();
    layout->addWidget(wrapper);
    setLayout(layout);
}

Odyssey::~Odyssey() {
    if (m_window) {
        delete m_window;
        m_window = nullptr;
    }
}

}  // namespace odyssey