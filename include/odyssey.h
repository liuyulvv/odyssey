#pragma once

/**
 * @file odyssey.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <QMainWindow>

#include "odyssey_keyboard_event.h"
#include "odyssey_model.h"
#include "odyssey_object.h"

namespace Ui {
class Odyssey;
}

namespace odyssey {

class OdysseyWindow;
class OdysseyDevice;
class OdysseyRender;
class OdysseyRenderSystem;
class OdysseyCamera;

class Odyssey : public QMainWindow {
public:
    Odyssey();
    ~Odyssey();
    Odyssey(const Odyssey& odyssey) = delete;
    Odyssey(Odyssey&& odyssey) = delete;
    Odyssey& operator=(const Odyssey& odyssey) = delete;
    Odyssey& operator=(Odyssey&& odyssey) = delete;

private:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

private:
    void draw();

public slots:
    void importObject();

private:
    void keyboardCallback(const OdysseyKeyboardEventType& event);
    void loadObject(const std::string& filePath);

private:
    void setupUI();
    void setupEngine();
    void setupEvent();
    void setupSignalsSlots();

private:
    OdysseyWindow* m_window{};
    Ui::Odyssey* ui;
    OdysseyDevice* m_device{};
    OdysseyRender* m_render{};
    std::vector<OdysseyObject> m_objects{};
    OdysseyRenderSystem* m_renderSystem{};
    OdysseyCamera* m_camera{};
};

}  // namespace odyssey