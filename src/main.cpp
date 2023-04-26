/**
 * @file main.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <QApplication>
#include <QWidget>

#include "odyssey.h"
#include "odyssey_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    odyssey::Odyssey odysseyApp;
    return app.exec();
}