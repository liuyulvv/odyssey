/**
 * @file main.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <QGuiApplication>
#include <QWidget>
#include <iostream>

#include "odyssey.h"
#include "odyssey_window.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    odyssey::OdysseyWindow window(800, 600, "123");
    // QWidget::createWindowContainer()
    // widget.show();
    // odyssey::Odyssey odyssey_app;
    // odyssey_app.Run();
    return app.exec();
}
