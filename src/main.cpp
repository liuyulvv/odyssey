/**
 * @file main.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QWidget>

#include "odyssey.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    // QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);
    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("odyssey", "Odyssey");
    auto* root = reinterpret_cast<QWindow*>(engine.rootObjects().at(0));
    // [[maybe_unused]] auto winId = root->winId();
    odyssey::Odyssey window;
    window.setParent(root);
    window.show();
    return app.exec();
}
