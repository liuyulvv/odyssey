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

#include "odyssey_device.h"
#include "odyssey_render.h"
#include "odyssey_render_system.h"
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
    m_device = new OdysseyDevice(m_window->getSurfaceInfo(), m_window->width(), m_window->height());
    m_render = new OdysseyRender(m_window, m_device);
    m_renderSystem = new OdysseyRenderSystem(m_device, m_render->getSwapChainRenderPass());
    show();
    draw();

    loadObject({
        {{0.F, -0.5F}, {1.F, 0.F, 0.F, 1.F}},
        {{-0.5F, 0.5F}, {0.F, 1.F, 0.F, 1.F}},
        {{0.5F, 0.5F}, {0.F, 0.F, 1.F, 1.F}},
    });
}

Odyssey::~Odyssey() {
    for (auto& object : m_objects) {
        object.model.reset();
    }
}

void Odyssey::paintEvent(QPaintEvent* event) {
    draw();
}

void Odyssey::resizeEvent(QResizeEvent* event) {
    draw();
}

void Odyssey::draw() {
    if (auto commandBuffer = m_render->beginFrame()) {
        m_render->beginSwapChainRenderPass(commandBuffer);
        m_renderSystem->renderObjects(commandBuffer, m_objects);
        m_render->endSwapChainRenderPass(commandBuffer);
        m_render->endFrame();
        update();
    }
}

void Odyssey::loadObject(const std::vector<OdysseyModel::Vertex>& vertices) {
    auto object = OdysseyObject::createObject();
    object.model = std::make_shared<OdysseyModel>(m_device, vertices);
    object.color = {1.F, 0.F, 0.F, 1.F};
    object.transform2D.translation.x = 0.2F;
    object.transform2D.scale = {2.F, 0.5F};
    object.transform2D.rotation = 0.25F * glm::two_pi<float>();
    m_objects.push_back(std::move(object));
}

}  // namespace odyssey