/**
 * @file odyssey.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include "odyssey.h"

namespace odyssey {

void Odyssey::Run() {
    InitialVulkan();
    MainLoop();
    Cleanup();
}

void Odyssey::InitialVulkan() {
}

void Odyssey::MainLoop() {
    while (!window_.ShouldClose()) {
        window_.PollEvents();
    }
}

void Odyssey::Cleanup() {
}

}  // namespace odyssey