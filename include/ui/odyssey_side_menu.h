#pragma once

/**
 * @file odyssey_side_menu.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-25
 */

#include <QWidget>

namespace Ui {
class OdysseySideMenu;
}

namespace odyssey {

class OdysseySideMenu : public QWidget {
public:
    OdysseySideMenu();
    ~OdysseySideMenu();
    OdysseySideMenu(const OdysseySideMenu& odyssey_side_menu) = delete;
    OdysseySideMenu(OdysseySideMenu&& odyssey_side_menu) = delete;
    OdysseySideMenu& operator=(const OdysseySideMenu& odyssey_side_menu) = delete;
    OdysseySideMenu& operator=(OdysseySideMenu&& odyssey_side_menu) = delete;

private:
    Ui::OdysseySideMenu* ui;
};

}  // namespace odyssey