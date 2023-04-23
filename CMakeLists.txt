cmake_minimum_required(VERSION 3.15)
project(odyssey LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
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
    set(CMAKE_CXX_FLAGS /utf-8)
    set(Vulkan_SDK "D:/VulkanSDK/1.3.236.0")
    set(CMAKE_PREFIX_PATH "D:/Qt/6.5.0/msvc2019_64/lib/cmake")
else()
    if (APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework IOKit")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
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

# glm
add_subdirectory(glm)

# assimp
option(ASSIMP_BUILD_TESTS "" OFF)
option(ASSIMP_INSTALL "" OFF)
add_subdirectory(assimp)

file(GLOB_RECURSE SRCS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

message(STATUS ${SRCS})

include_directories(
    ${Vulkan_INCLUDE_DIRS}
    "include"
    "include/event"
    "glm"
    "assimp/include"
    ${Qt6Gui_PRIVATE_INCLUDE_DIRS}
)

link_directories(
    ${Vulkan_LIBRARY}
    ${EXECUTABLE_OUTPUT_PATH}/lib
)

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} 
PRIVATE  
    ${Vulkan_LIBRARIES}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    assimp::assimp
)

if (MSVC)
    target_compile_options(
        assimp PRIVATE 
        /EHsc /W4 /WX
    )
    target_compile_options(
        ${PROJECT_NAME} PRIVATE 
        /EHsc /W4 /WX
    )
endif()