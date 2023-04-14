/**
 * @file odyssey.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-12
 */

#if !defined(ODYSSEY_H_)
#define ODYSSEY_H_

#include <QWidget>

namespace odyssey {

class OdysseyWindow;

class Odyssey : public QWidget {
public:
    Odyssey();
    ~Odyssey();
    Odyssey(const Odyssey& odyssey_gui) = delete;
    Odyssey(Odyssey&& odyssey_gui) = delete;
    Odyssey& operator=(const Odyssey& odyssey_gui) = delete;
    Odyssey& operator=(Odyssey&& odyssey_gui) = delete;

private:
    OdysseyWindow* m_window{nullptr};
};

}  // namespace odyssey

#endif  // ODYSSEY_H_