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

#include "odyssey_camera.h"
#include "odyssey_device.h"
#include "odyssey_render.h"
#include "odyssey_render_system.h"
#include "odyssey_window.h"

namespace odyssey {

Odyssey::Odyssey() : m_window(new OdysseyWindow()) {
    m_window->setMouseCallback([this](OdysseyMouseEvent event) {
        if (event.type == OdysseyMouseEventType::LEFT_DOUBLE) {
        } else if (event.type == OdysseyMouseEventType::LEFT) {
        } else if (event.type == OdysseyMouseEventType::RIGHT) {
        }
    });
    setMinimumHeight(600);
    setMinimumWidth(800);
    auto* wrapper = QWidget::createWindowContainer(m_window);
    wrapper->setFocusPolicy(Qt::StrongFocus);
    wrapper->setFocus();
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(wrapper, 0);
    setLayout(layout);
    m_device = new OdysseyDevice(m_window->getSurfaceInfo());
    m_render = new OdysseyRender(m_window, m_device);
    m_renderSystem = new OdysseyRenderSystem(m_device, m_render->getSwapChainRenderPass());
    m_camera = new OdysseyCamera();
    m_camera->setViewDirection(glm::vec3(0.0F), glm::vec3(0.5F, 0.0F, 1.0F));
    show();
    draw();
    loadObject();
}

Odyssey::~Odyssey() {
    for (auto& object : m_objects) {
        object.model.reset();
    }
}

void Odyssey::paintEvent([[maybe_unused]] QPaintEvent* event) {
    draw();
}

void Odyssey::resizeEvent([[maybe_unused]] QResizeEvent* event) {
    draw();
}

void Odyssey::draw() {
    if (auto commandBuffer = m_render->beginFrame()) {
        auto aspect = m_render->getAspectRatio();
        // m_camera->setOrthographicProjection(-aspect, aspect, -1.0F, 1.0F, -1.0F, 1.0F);
        m_camera->setPerspectiveProjection(glm::radians(50.0F), aspect, 0.1F, 10.0F);
        m_render->beginSwapChainRenderPass(commandBuffer);
        m_renderSystem->renderObjects(commandBuffer, m_objects, m_camera);
        m_render->endSwapChainRenderPass(commandBuffer);
        m_render->endFrame();
        update();
    }
}

void Odyssey::loadObject() {
    auto model = createCubeModel(m_device, {0.0F, 0.0F, 0.0F});
    auto object = OdysseyObject::createObject();
    object.model = model;
    object.transform.translation = {0.0F, 0.0F, 2.5F};
    object.transform.scale = {0.5F, 0.5F, 0.5F};
    m_objects.push_back(std::move(object));
}

std::shared_ptr<OdysseyModel> Odyssey::createCubeModel(OdysseyDevice* device, glm::vec3 offset) {
    OdysseyModel::Builder builder{};
    builder.vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };
    for (auto& v : builder.vertices) {
        v.position += offset;
    }
    builder.indices = {0, 1, 2, 0, 3, 1, 4, 5, 6, 4, 7, 5, 8, 9, 10, 8, 11, 9, 12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};
    return std::make_shared<OdysseyModel>(device, builder);
}

}  // namespace odyssey