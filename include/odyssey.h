#pragma once

/**
 * @file odyssey.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <QWidget>
#include <memory>

#include "odyssey_model.h"
#include "odyssey_object.h"

namespace odyssey {

class OdysseyWindow;
class OdysseyDevice;
class OdysseyRender;
class OdysseyRenderSystem;

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
    void loadObject();

private:
    void draw();
    std::shared_ptr<OdysseyModel> createCubeModel(OdysseyDevice* device, glm::vec3 offset);

private:
    OdysseyWindow* m_window{};
    OdysseyDevice* m_device{};
    OdysseyRender* m_render{};
    std::vector<OdysseyObject> m_objects{};
    OdysseyRenderSystem* m_renderSystem{};
};

}  // namespace odyssey