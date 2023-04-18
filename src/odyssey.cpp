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
#include "odyssey_window.h"

namespace odyssey {

Odyssey::Odyssey() : window_(new OdysseyWindow()) {
    setMinimumHeight(600);
    setMinimumWidth(800);
    auto* wrapper = QWidget::createWindowContainer(window_);
    wrapper->setFocusPolicy(Qt::StrongFocus);
    wrapper->setFocus();
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(wrapper, 0);
    setLayout(layout);
    engine_ = new OdysseyEngine(window_->GetSurfaceInfo(), window_->width(), window_->height());
    show();
    resize(800, 600);
}

Odyssey::~Odyssey() {
    delete engine_;
    delete window_;
}

void Odyssey::paintEvent(QPaintEvent* event) {
    Draw();
}

void Odyssey::resizeEvent(QResizeEvent* event) {
    Draw();
}

void Odyssey::Draw() {
    try {
        if (engine_) {
            auto image_index = engine_->AcquireNextImage();
            engine_->RecordCommandBuffer(image_index);
            engine_->SubmitCommandBuffers(image_index);
        }
    } catch ([[maybe_unused]] const vk::OutOfDateKHRError& e) {
        engine_->RecreateSwapChain(window_->width(), window_->height());
    }
}

}  // namespace odyssey