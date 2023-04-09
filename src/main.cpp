/**
 * @file main.cpp
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-09
 */

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "odyssey.h"

int main() {
    try {
        odyssey::Odyssey app;
        app.Run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
