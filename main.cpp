#include "application/Application.h"
#include "application/backends/RaylibPlatform.h"
#include "renderer/backends/RaylibRenderDevice.h"

int main() {
    application::backends::RaylibPlatform platform;
    renderer::backends::RaylibRenderDevice renderDevice;

    application::Application app(platform, renderDevice);

    if (!app.Initialize(1280, 720, "Mustown")) {
        return 1;
    }

    app.Run();

    return 0;
}