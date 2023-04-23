#pragma once

/**
 * @file odyssey_mouse_controller.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-23
 */

namespace odyssey {

class OdysseyMouseController {
public:
    OdysseyMouseController() = default;
    ~OdysseyMouseController() = default;
    OdysseyMouseController(const OdysseyMouseController& odysseyMouseController) = default;
    OdysseyMouseController(OdysseyMouseController&& odysseyMouseController) = default;
    OdysseyMouseController& operator=(const OdysseyMouseController& odysseyMouseController) = default;
    OdysseyMouseController& operator=(OdysseyMouseController&& odysseyMouseController) = default;
};

}  // namespace odyssey