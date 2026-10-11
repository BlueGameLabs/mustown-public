#ifndef MUSTOWN_RAYLIBPLATFORM_H
#define MUSTOWN_RAYLIBPLATFORM_H

#include "../Platform.h"

namespace application::backends {

    class RaylibPlatform final : public application::Platform {
    public:
        bool Initialize(
            int width,
            int height,
            const char* title
        ) override;

        bool ShouldClose() const override;

        float GetDeltaTime() const override;

        void BeginFrame() override;

        void EndFrame() override;

        void Shutdown() override;

    private:
        bool initialized = false;
    };

} // namespace application::backends

#endif // MUSTOWN_RAYLIBPLATFORM_H