#include "Application.h"

#include "../renderer/SpriteRenderer.h"

namespace application {

    Application::Application(
        Platform& platform,
        renderer::RenderDevice& renderDevice
    )
        : platform(platform),
          renderDevice(renderDevice) {}

    Application::~Application() {
        Shutdown();
    }

    bool Application::Initialize(
        int width,
        int height,
        const char* title
    ) {
        if (initialized || width <= 0 || height <= 0 || title == nullptr) {
            return false;
        }

        if (!platform.Initialize(width, height, title)) {
            return false;
        }

        spriteRenderer =
            std::make_unique<renderer::SpriteRenderer>(renderDevice);

        initialized = true;
        running = true;

        return true;
    }

    void Application::Run() {
        if (!initialized || !running) {
            return;
        }

        while (running && !platform.ShouldClose()) {
            const float deltaTime = platform.GetDeltaTime();

            Update(deltaTime);

            platform.BeginFrame();
            Render();
            platform.EndFrame();
        }

        running = false;
    }

    void Application::Update(float deltaTime) {
        // Update the scheduler, game systems, and animation controllers.
        (void)deltaTime;
    }

    void Application::Render() {
        // Render scenes and other game systems here.
    }

    void Application::Shutdown() {
        if (!initialized) {
            return;
        }

        running = false;

        // Release sprite-owned GPU resources before shutting down the platform.
        spriteRenderer.reset();

        platform.Shutdown();

        initialized = false;
    }

    bool Application::IsRunning() const {
        return running;
    }

} // namespace application