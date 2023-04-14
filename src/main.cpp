/**
 * @file main.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <QApplication>

#include "odyssey.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    odyssey::Odyssey odysseyAPP;
    odysseyAPP.resize(1024, 768);
    odysseyAPP.show();
    return app.exec();
}