/**
 * @file main.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <QApplication>
#include <QWidget>
#include <iostream>

#include "odyssey.h"
#include "odyssey_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    odyssey::Odyssey odyssey_app;
    odyssey_app.resize(800, 600);
    odyssey_app.show();
    return app.exec();
}
