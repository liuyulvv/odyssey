/**
 * @file odyssey_side_menu.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-25
 */

#include "odyssey_side_menu.h"

#include "ui_odyssey_side_menu.h"

namespace odyssey {

OdysseySideMenu::OdysseySideMenu() : ui(new Ui::OdysseySideMenu) {
    ui->setupUi(this);
    setLayout(ui->verticalLayout);
}

OdysseySideMenu::~OdysseySideMenu() {
    delete ui;
}

}  // namespace odyssey