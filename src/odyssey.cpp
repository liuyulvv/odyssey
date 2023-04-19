/**
 * @file odyssey.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey.h"

#include <qpa/qplatformnativeinterface.h>

#include <QGuiApplication>
#include <QVBoxLayout>
#include <array>
#include <stdexcept>

#include "odyssey_engine.h"
#include "odyssey_mouse_event.h"
#include "odyssey_window.h"

namespace odyssey {

Odyssey::Odyssey() : m_window(new OdysseyWindow()) {
    setMinimumHeight(600);
    setMinimumWidth(800);
    auto* wrapper = QWidget::createWindowContainer(m_window);
    wrapper->setFocusPolicy(Qt::StrongFocus);
    wrapper->setFocus();
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(wrapper, 0);
    setLayout(layout);
    m_engine = new OdysseyEngine(m_window->getSurfaceInfo(), m_window->width(), m_window->height());
    show();
    draw();

    m_window->setMouseCallback([this](OdysseyMouseEvent event) {
        if (event.type == OdysseyMouseEventType::LEFT_DOUBLE) {
        } else if (event.type == OdysseyMouseEventType::LEFT) {
            this->m_vertices.push_back({{event.position.worldX, event.position.worldY}, {1.0F, 0.0F, 0.0F, 1.0F}});
        } else if (event.type == OdysseyMouseEventType::RIGHT) {
            m_engine->loadModel(this->m_vertices);
            this->m_vertices.clear();
            update();
        }
    });
}

Odyssey::~Odyssey() {
    delete m_engine;
    delete m_window;
}

void Odyssey::paintEvent(QPaintEvent* event) {
    draw();
}

void Odyssey::resizeEvent(QResizeEvent* event) {
    draw();
}

void Odyssey::draw() {
    try {
        if (m_engine) {
            auto imageIndex = m_engine->acquireNextImage();
            m_engine->recordCommandBuffer(imageIndex);
            m_engine->submitCommandBuffers(imageIndex);
        }
    } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
        m_engine->recreateSwapChain(m_window->width(), m_window->height());
    }
}

}  // namespace odyssey