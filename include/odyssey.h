/**
 * @file odyssey.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#if !defined(ODYSSEY_ODYSSEY_H_)
#define ODYSSEY_ODYSSEY_H_

#include <QWidget>

namespace odyssey {

class OdysseyWindow;
class OdysseyEngine;

class Odyssey : public QWidget {
public:
    Odyssey();
    ~Odyssey();
    Odyssey(const Odyssey& odyssey) = delete;
    Odyssey(Odyssey&& odyssey) = delete;
    Odyssey& operator=(const Odyssey& odyssey) = delete;
    Odyssey& operator=(Odyssey&& odyssey) = delete;

public:
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);

private:
    void Draw();

private:
    OdysseyWindow* window_{nullptr};
    OdysseyEngine* engine_{nullptr};
};

}  // namespace odyssey

#endif  // ODYSSEY_ODYSSEY_H_