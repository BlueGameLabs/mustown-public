cmake_minimum_required(VERSION 3.11)
project(mustown)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies

# raylib
set(RAYLIB_VERSION 6.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)

if (NOT raylib_FOUND)
include(FetchContent)
FetchContent_Declare(
raylib
DOWNLOAD_EXTRACT_TIMESTAMP OFF
URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
)
FetchContent_GetProperties(raylib)
if (NOT raylib_POPULATED)
set(FETCHCONTENT_QUIET NO)
FetchContent_MakeAvailable(raylib)
endif()
endif()

# spdlog
set(SPDLOG_VERSION 1.15.3)
find_package(spdlog ${SPDLOG_VERSION} QUIET)

if (NOT spdlog_FOUND)
include(FetchContent)
FetchContent_Declare(
spdlog
DOWNLOAD_EXTRACT_TIMESTAMP OFF
URL https://github.com/gabime/spdlog/archive/refs/tags/v${SPDLOG_VERSION}.tar.gz
)
FetchContent_MakeAvailable(spdlog)
endif()

# Our Project
add_executable(${PROJECT_NAME} main.cpp
        renderer/renderer.h
        renderer/SpriteRenderer.cpp
        renderer/SpriteRenderer.h
        renderer/atlas/TextureAtlas.cpp
        renderer/atlas/TextureAtlas.h
        renderer/atlas/AtlasRegion.cpp
        renderer/atlas/AtlasRegion.h
        renderer/RenderDevice.h
        renderer/backends/RaylibRenderDevice.cpp
        renderer/backends/RaylibRenderDevice.h
        renderer/animation/Animation.cpp
        renderer/animation/Animation.h
        renderer/animation/AnimationController.cpp
        renderer/animation/AnimationController.h
        application/Application.cpp
        application/Application.h
        application/platform.h
        application/backends/RaylibPlatform.cpp
        application/backends/RaylibPlatform.h
        scheduler/ScheduledTask.cpp
        scheduler/ScheduledTask.h
        scheduler/SchedulerConfig.cpp
        scheduler/SchedulerConfig.h
        scheduler/TaskQueue.cpp
        scheduler/TaskQueue.h
        scheduler/SchedulerModule.cpp
        scheduler/SchedulerModule.h
        scheduler/scheduler.cpp
        scheduler/scheduler.h)

target_link_libraries(${PROJECT_NAME}
PRIVATE
raylib
spdlog::spdlog
)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
set(CMAKE_EXE_LINKER_FLAGS
"${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY"
)
endif()

# Checks if OSX and links appropriate frameworks
if (APPLE)
target_link_libraries(${PROJECT_NAME}
PRIVATE
"-framework IOKit"
"-framework Cocoa"
"-framework OpenGL"
"-framework QuartzCore"
)
endif()
