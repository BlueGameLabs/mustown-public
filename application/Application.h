#ifndef MUSTOWN_APPLICATION_H
#define MUSTOWN_APPLICATION_H

#include "../renderer/RenderDevice.h"
#include "platform.h"

#include <memory>

namespace renderer {
    class SpriteRenderer;
}

namespace application {

    class Application {
    public:
        Application(
            Platform& platform,
            renderer::RenderDevice& renderDevice
        );

        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        bool Initialize(int width, int height, const char* title);

        void Run();

        void Shutdown();

        bool IsRunning() const;

    private:
        Platform& platform;
        renderer::RenderDevice& renderDevice;

        std::unique_ptr<renderer::SpriteRenderer> spriteRenderer;

        bool initialized = false;
        bool running = false;

        void Update(float deltaTime);
        void Render();
    };

} // namespace application

#endif // MUSTOWN_APPLICATION_H