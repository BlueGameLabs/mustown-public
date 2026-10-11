#include "Animation.h"

#include <utility>

namespace renderer::animation {

    Animation::Animation(
        std::vector<std::string> frames,
        float framesPerSecond,
        bool looping
    )
        : frames(std::move(frames)),
          framesPerSecond(framesPerSecond),
          looping(looping) {}

    const std::vector<std::string>& Animation::GetFrames() const {
        return frames;
    }

    float Animation::GetFramesPerSecond() const {
        return framesPerSecond;
    }

    bool Animation::IsLooping() const {
        return looping;
    }

    std::size_t Animation::GetFrameCount() const {
        return frames.size();
    }

} // namespace renderer::animation