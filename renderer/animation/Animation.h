#ifndef MUSTOWN_ANIMATION_H
#define MUSTOWN_ANIMATION_H

#include <string>
#include <vector>

namespace renderer::animation {

    class Animation {
    public:
        Animation(
            std::vector<std::string> frames,
            float framesPerSecond,
            bool looping = true
        );

        const std::vector<std::string>& GetFrames() const;

        float GetFramesPerSecond() const;

        bool IsLooping() const;

        std::size_t GetFrameCount() const;

    private:
        std::vector<std::string> frames;
        float framesPerSecond;
        bool looping;
    };

} // namespace renderer::animation

#endif // MUSTOWN_ANIMATION_H