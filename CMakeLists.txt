cmake_minimum_required(VERSION 3.15)
project(odyssey LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/lib)

set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")
if(NOT ${IS_DEBUG_BUILD})
    add_compile_definitions(NODEBUG)
endif()

# GLFW
option(BUILD_SHARED_LIBS "" ON)
add_subdirectory(glfw)

# FMT
add_subdirectory(fmt)

if (MSVC)
    add_compile_options(/W4)
    set(CMAKE_CXX_FLAGS /source-charset:utf-8)
    include_directories(
        "D:/VulkanSDK/1.3.236.0/Include"
    )
    link_directories(
        "D:/VulkanSDK/1.3.236.0/Lib"
    )
else()
    if (APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework IOKit")
        include_directories(
            "/usr/local/include"
        )
        link_directories(
            "/usr/local/lib"
        )
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

file(GLOB SRCS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

include_directories(
    "include"
    "glfw/include"
    "fmt/include"
)

link_directories(
    ${EXECUTABLE_OUTPUT_PATH}/lib
)

if (MSVC)
    add_executable(${PROJECT_NAME} ${SRCS})
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS" COMPILE_FLAGS "/EHsc")
    target_link_libraries(${PROJECT_NAME} vulkan-1 glfw3dll fmt::fmt)
elseif (APPLE)
    add_executable(${PROJECT_NAME} ${SRCS})
    target_link_libraries(${PROJECT_NAME} vulkan glfw fmt::fmt)
endif()