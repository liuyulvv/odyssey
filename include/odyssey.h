#pragma once

/**
 * @file odyssey.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <QWidget>

#include "odyssey_model.h"

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
    void draw();

private:
    OdysseyWindow* m_window{nullptr};
    OdysseyEngine* m_engine{nullptr};

    std::vector<OdysseyModel::Vertex> m_vertices;
};

}  // namespace odyssey