#pragma once

/**
 * @file odyssey_header.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-21
 */

#if defined(_WIN32)
#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // VK_USE_PLATFORM_WIN32_KHR
#endif
#include "vulkan/vulkan.hpp"

#if !defined(GLM_FORCE_RADIANS)
#define GLM_FORCE_RADIANS
#endif  // GLM_FORCE_RADIANS

#if !defined(GLM_DEPTH_ZERO_TO_ONE)
#define GLM_DEPTH_ZERO_TO_ONE
#endif  // GLM_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"