#ifndef MUSTOWN_ANIMATIONCONTROLLER_H
#define MUSTOWN_ANIMATIONCONTROLLER_H

#include <cstddef>
#include <string>
#include <unordered_map>

#include "Animation.h"

namespace renderer::animation {

    class AnimationController {
    public:
        AnimationController() = default;

        bool AddAnimation(
            const std::string& name,
            Animation animation
        );

        bool HasAnimation(const std::string& name) const;

        bool Play(const std::string& name);

        void Pause();

        void Resume();

        void Stop();

        void Update(float deltaTime);

        const std::string* GetCurrentFrame() const;

        const std::string* GetCurrentAnimationName() const;

        bool IsPlaying() const;

        bool IsPaused() const;

        bool IsFinished() const;

        std::size_t GetCurrentFrameIndex() const;

        void Clear();

    private:
        std::unordered_map<std::string, Animation> animations;

        std::string currentAnimationName;

        std::size_t currentFrameIndex = 0;
        double frameTimer = 0.0;

        bool playing = false;
        bool paused = false;
        bool finished = false;

        const Animation* GetCurrentAnimation() const;
    };

} // namespace renderer::animation

#endif // MUSTOWN_ANIMATIONCONTROLLER_H