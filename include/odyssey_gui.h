/**
 * @file odyssey_gui.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-12
 */

#if !defined(ODYSSEY_GUI_H_)
#define ODYSSEY_GUI_H_

#include <QWidget>

namespace odyssey {

class OdysseyWindow;

class OdysseyGUI : public QWidget {
public:
    explicit OdysseyGUI(OdysseyWindow* window);
    ~OdysseyGUI() = default;
    OdysseyGUI(const OdysseyGUI& odyssey_gui) = delete;
    OdysseyGUI(OdysseyGUI&& odyssey_gui) = delete;
    OdysseyGUI& operator=(const OdysseyGUI& odyssey_gui) = delete;
    OdysseyGUI& operator=(OdysseyGUI&& odyssey_gui) = delete;
};

}  // namespace odyssey

#endif  // ODYSSEY_GUI_H_