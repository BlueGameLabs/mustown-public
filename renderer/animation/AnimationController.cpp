#include "AnimationController.h"

#include <cmath>
#include <utility>

namespace renderer::animation {

    bool AnimationController::AddAnimation(
        const std::string& name,
        Animation animation
    ) {
        if (name.empty() ||
            animation.GetFrameCount() == 0 ||
            !std::isfinite(animation.GetFramesPerSecond()) ||
            animation.GetFramesPerSecond() <= 0.0f) {
            return false;
        }

        for (const std::string& frame : animation.GetFrames()) {
            if (frame.empty()) {
                return false;
            }
        }

        return animations.emplace(
            name,
            std::move(animation)
        ).second;
    }

    bool AnimationController::HasAnimation(
        const std::string& name
    ) const {
        return animations.find(name) != animations.end();
    }

    bool AnimationController::Play(const std::string& name) {
        const auto it = animations.find(name);

        if (it == animations.end()) {
            return false;
        }

        // Restart playback from the first frame.
        currentAnimationName = name;
        currentFrameIndex = 0;
        frameTimer = 0.0;

        playing = true;
        paused = false;
        finished = false;

        return true;
    }

    void AnimationController::Pause() {
        if (playing && !finished) {
            paused = true;
        }
    }

    void AnimationController::Resume() {
        if (playing && paused && !finished) {
            paused = false;
        }
    }

    void AnimationController::Stop() {
        playing = false;
        paused = false;
        finished = false;

        currentAnimationName.clear();
        currentFrameIndex = 0;
        frameTimer = 0.0;
    }

    void AnimationController::Update(float deltaTime) {
        if (!playing ||
            paused ||
            finished ||
            !std::isfinite(deltaTime) ||
            deltaTime <= 0.0f) {
            return;
        }

        const Animation* animation = GetCurrentAnimation();

        if (animation == nullptr) {
            Stop();
            return;
        }

        const double frameDuration =
            1.0 / animation->GetFramesPerSecond();

        frameTimer += deltaTime;

        const auto elapsedFrames = static_cast<std::size_t>(
            frameTimer / frameDuration
        );

        if (elapsedFrames == 0) {
            return;
        }

        frameTimer = std::fmod(frameTimer, frameDuration);

        const std::size_t frameCount = animation->GetFrameCount();

        if (animation->IsLooping()) {
            currentFrameIndex =
                (currentFrameIndex + elapsedFrames % frameCount)
                % frameCount;
        } else {
            const std::size_t remainingFrames =
                frameCount - 1 - currentFrameIndex;

            if (elapsedFrames >= remainingFrames) {
                currentFrameIndex = frameCount - 1;
                frameTimer = 0.0;
                finished = true;
                playing = false;
            } else {
                currentFrameIndex += elapsedFrames;
            }
        }
    }

    const std::string* AnimationController::GetCurrentFrame() const {
        const Animation* animation = GetCurrentAnimation();

        if (animation == nullptr ||
            currentFrameIndex >= animation->GetFrameCount()) {
            return nullptr;
        }

        return &animation->GetFrames()[currentFrameIndex];
    }

    const std::string*
    AnimationController::GetCurrentAnimationName() const {
        if (!playing && !finished && currentAnimationName.empty()) {
            return nullptr;
        }

        if (currentAnimationName.empty()) {
            return nullptr;
        }

        return &currentAnimationName;
    }

    bool AnimationController::IsPlaying() const {
        return playing && !paused;
    }

    bool AnimationController::IsPaused() const {
        return paused;
    }

    bool AnimationController::IsFinished() const {
        return finished;
    }

    std::size_t AnimationController::GetCurrentFrameIndex() const {
        return currentFrameIndex;
    }

    const Animation* AnimationController::GetCurrentAnimation() const {
        const auto it = animations.find(currentAnimationName);

        if (it == animations.end()) {
            return nullptr;
        }

        return &it->second;
    }

    void AnimationController::Clear() {
        Stop();
        animations.clear();
    }

} // namespace renderer::animation