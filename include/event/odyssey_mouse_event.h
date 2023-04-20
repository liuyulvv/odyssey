#pragma once

/**
 * @file odyssey_mouse_event.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-19
 */

namespace odyssey {

enum class OdysseyMouseEventType {
    LEFT,
    MID,
    RIGHT,
    LEFT_MOVE,
    MID_MOVE,
    RIGHT_MOVE,
    LEFT_DOUBLE,
};

struct OdysseyMousePosition {
    int screenX;
    int screenY;
    float worldX;
    float worldY;
};

struct OdysseyMouseEvent {
    OdysseyMouseEventType type;
    OdysseyMousePosition position;
};

static void screenToWorld(int screenX, int screenY, int width, int height, float& worldX, float& worldY) {
    worldX = static_cast<float>(screenX) / width * 2.0F - 1.0F;
    worldY = static_cast<float>(screenY) / height * 2.0F - 1.0F;
}

}  // namespace odyssey