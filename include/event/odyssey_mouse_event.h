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

}  // namespace odyssey