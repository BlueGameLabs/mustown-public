#include "RaylibPlatform.h"

#include <raylib.h>

namespace application::backends {

    bool RaylibPlatform::Initialize(
        int width,
        int height,
        const char* title
    ) {
        if (initialized) {
            return false;
        }

        ::InitWindow(width, height, title);

        if (!::IsWindowReady()) {
            return false;
        }

        initialized = true;
        return true;
    }

    bool RaylibPlatform::ShouldClose() const {
        return !initialized || ::WindowShouldClose();
    }

    float RaylibPlatform::GetDeltaTime() const {
        return initialized ? ::GetFrameTime() : 0.0f;
    }

    void RaylibPlatform::BeginFrame() {
        if (!initialized) {
            return;
        }

        ::BeginDrawing();
        ::ClearBackground(BLACK);
    }

    void RaylibPlatform::EndFrame() {
        if (initialized) {
            ::EndDrawing();
        }
    }

    void RaylibPlatform::Shutdown() {
        if (!initialized) {
            return;
        }

        ::CloseWindow();
        initialized = false;
    }

} // namespace application::backends