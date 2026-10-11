#ifndef MUSTOWN_PLATFORM_H
#define MUSTOWN_PLATFORM_H

namespace application {

    class Platform {
    public:
        virtual ~Platform() = default;

        virtual bool Initialize(
            int width,
            int height,
            const char* title
        ) = 0;

        virtual bool ShouldClose() const = 0;

        virtual float GetDeltaTime() const = 0;

        virtual void BeginFrame() = 0;

        virtual void EndFrame() = 0;

        virtual void Shutdown() = 0;
    };

} // namespace application

#endif // MUSTOWN_PLATFORM_H