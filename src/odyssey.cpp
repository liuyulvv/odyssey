/**
 * @file odyssey.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey.h"

#include <QKeyEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <stdexcept>

#include "odyssey_camera.h"
#include "odyssey_device.h"
#include "odyssey_render.h"
#include "odyssey_render_system.h"
#include "odyssey_window.h"
#include "ui_odyssey.h"

namespace odyssey {

Odyssey::Odyssey() : m_window(new OdysseyWindow()), ui(new Ui::Odyssey) {
    // setup ui
    ui->setupUi(this);
    auto* wrapper = QWidget::createWindowContainer(m_window, this);
    ui->horizontalLayout->addWidget(wrapper);
    ui->centralWidget->setLayout(ui->horizontalLayout);
    // init odyssey
    m_device = new OdysseyDevice(m_window->getSurfaceInfo());
    m_render = new OdysseyRender(m_window, m_device);
    m_renderSystem = new OdysseyRenderSystem(m_device, m_render->getSwapChainRenderPass());
    m_camera = new OdysseyCamera();
    m_camera->setViewDirection(glm::vec3(0.0F), glm::vec3(0.0F, 0.0F, 1.0F));
    // window mouse event callback
    m_window->setMouseCallback([this](OdysseyMouseEvent event) {
        if (event.type == OdysseyMouseEventType::LEFT_DOUBLE) {
        } else if (event.type == OdysseyMouseEventType::LEFT) {
        } else if (event.type == OdysseyMouseEventType::RIGHT) {
        }
    });
    // connect signals and slots
    connect(ui->actionImport, &QAction::triggered, this, &Odyssey::importObject);
    show();
}

Odyssey::~Odyssey() {
    delete ui;
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

void Odyssey::keyPressEvent(QKeyEvent* event) {
    OdysseyKeyboardEventType type{};
    switch (event->key()) {
        case Qt::Key_W:
            type = OdysseyKeyboardEventType::W;
            break;
        case Qt::Key_A:
            type = OdysseyKeyboardEventType::A;
            break;
        case Qt::Key_S:
            type = OdysseyKeyboardEventType::S;
            break;
        case Qt::Key_D:
            type = OdysseyKeyboardEventType::D;
            break;
        case Qt::Key_Up:
            type = OdysseyKeyboardEventType::UP;
            break;
        case Qt::Key_Down:
            type = OdysseyKeyboardEventType::DOWN;
            break;
        case Qt::Key_Left:
            type = OdysseyKeyboardEventType::LEFT;
            break;
        case Qt::Key_Right:
            type = OdysseyKeyboardEventType::RIGHT;
            break;
    }
}

void Odyssey::draw() {
    if (auto commandBuffer = m_render->beginFrame()) {
        auto aspect = m_render->getAspectRatio();
        m_camera->setPerspectiveProjection(glm::radians(50.0F), aspect, 0.1F, 10.0F);
        m_render->beginSwapChainRenderPass(commandBuffer);
        m_renderSystem->renderObjects(commandBuffer, m_objects, m_camera);
        m_render->endSwapChainRenderPass(commandBuffer);
        m_render->endFrame();
        update();
    }
}

void Odyssey::importObject() {
    auto filePath = QFileDialog::getOpenFileName(this, "导入", "", "*.obj");
    if (!filePath.isEmpty())
        loadObject(filePath.toStdString());
}

void Odyssey::keyboardCallback([[maybe_unused]] const OdysseyKeyboardEventType& event) {
}

void Odyssey::loadObject(const std::string& filePath) {
    std::shared_ptr<OdysseyModel> model = OdysseyModel::createModelFromFile(m_device, filePath);
    auto object = OdysseyObject::createObject();
    object.model = model;
    object.transform.translation = {0.0F, 0.0F, 1.0F};
    m_objects.push_back(std::move(object));
}

}  // namespace odyssey