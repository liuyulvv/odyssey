/**
 * @file odyssey.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey.h"

namespace odyssey {

void Odyssey::Run() {
    MainLoop();
}

void Odyssey::MainLoop() {
    while (!window_.ShouldClose()) {
        window_.PollEvents();
    }
}

}  // namespace odyssey