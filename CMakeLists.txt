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

if (MSVC)
    add_compile_options(/W4)
    set(CMAKE_CXX_FLAGS /utf-8)
    set(Vulkan_SDK "D:/VulkanSDK/1.3.236.0")
    set(CMAKE_PREFIX_PATH "D:/Qt/6.5.0/msvc2019_64/lib/cmake")
else()
    if (APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework IOKit")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    add_compile_options(-Wall -Wextra -pedantic)
endif()

find_package(Qt6 REQUIRED COMPONENTS Concurrent Core Gui Widgets)
qt_standard_project_setup()

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

# glslc compile shader
file(GLOB shaders ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag)
foreach(shader IN LISTS shaders)
    get_filename_component(filename ${shader} NAME ABSOLUTE)
    add_custom_command(
        COMMAND
        ${glslc_executable}
        -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${filename}.spv
        ${shader}
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${filename}.spv
        DEPENDS ${shader} ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        COMMENT "Compiling ${filename}"
    )
    list(APPEND spv_shaders ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${filename}.spv)
endforeach()
add_custom_target(shaders ALL DEPENDS ${spv_shaders})

file(GLOB SRCS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

include_directories(
    "include"
)

link_directories(
)

if (MSVC)
    add_executable(${PROJECT_NAME} ${SRCS} ${IMGUI_SRCS})
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        COMPILE_FLAGS "/EHsc"
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
    )
    target_link_libraries(${PROJECT_NAME} 
        PRIVATE  
                Qt6::Concurrent
                Qt6::Core
                Qt6::Gui
                Qt6::Widgets
    )
elseif (APPLE)
    add_executable(${PROJECT_NAME} ${SRCS} ${IMGUI_SRCS})
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
    )
    target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        Qt6::Concurrent
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
    )
endif()