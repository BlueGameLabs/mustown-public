#include "SpriteRenderer.h"

#include <utility>

namespace renderer {

    SpriteRenderer::SpriteRenderer(RenderDevice& device)
        : device(device) {}

    SpriteRenderer::~SpriteRenderer() {
        Unload();
    }

    bool SpriteRenderer::Load(
        const std::string& texturePath,
        const std::string& atlasPath
    ) {
        // Load metadata first so failure doesn't replace the current atlas.
        atlas::TextureAtlas newAtlas;

        if (!newAtlas.LoadFromFile(atlasPath)) {
            return false;
        }

        TextureHandle newTexture = 0;

        if (!device.LoadTexture(texturePath, newTexture)) {
            return false;
        }

        // Replace the old resources only after both loads succeed.
        Unload();

        atlas = std::move(newAtlas);
        texture = newTexture;
        loaded = true;

        return true;
    }

    void SpriteRenderer::Unload() {
        if (loaded) {
            device.UnloadTexture(texture);
        }

        texture = 0;
        loaded = false;
        atlas.Clear();

        animations.clear();
        ResetAnimationState();
    }

    bool SpriteRenderer::IsLoaded() const {
        return loaded;
    }

    bool SpriteRenderer::DrawSprite(
        const std::string& name,
        float x,
        float y
    ) {
        if (!loaded) {
            return false;
        }

        const atlas::AtlasRegion* region = atlas.GetRegion(name);

        if (region == nullptr) {
            return false;
        }

        device.DrawTextureRegion(texture, *region, x, y);
        return true;
    }

    bool SpriteRenderer::AddAnimation(
        const std::string& name,
        const std::vector<std::string>& frames,
        float framesPerSecond,
        bool looping
    ) {
        if (!loaded ||
            name.empty() ||
            frames.empty() ||
            framesPerSecond <= 0.0f) {
            return false;
        }

        // Reject animations that reference missing atlas regions.
        for (const std::string& frame : frames) {
            if (!atlas.HasRegion(frame)) {
                return false;
            }
        }

        Animation animation;
        animation.frames = frames;
        animation.framesPerSecond = framesPerSecond;
        animation.looping = looping;

        return animations.emplace(name, std::move(animation)).second;
    }

    bool SpriteRenderer::PlayAnimation(const std::string& name) {
        if (!loaded) {
            return false;
        }

        const auto it = animations.find(name);

        if (it == animations.end()) {
            return false;
        }

        currentAnimationName = name;
        currentAnimation = &it->second;
        currentFrame = 0;
        frameTimer = 0.0f;

        return true;
    }

    void SpriteRenderer::StopAnimation() {
        ResetAnimationState();
    }

    void SpriteRenderer::ResetAnimationState() {
        currentAnimation = nullptr;
        currentAnimationName.clear();
        currentFrame = 0;
        frameTimer = 0.0f;
    }

    void SpriteRenderer::Update(float deltaTime) {
        if (!loaded ||
            currentAnimation == nullptr ||
            deltaTime <= 0.0f) {
            return;
        }

        frameTimer += deltaTime;

        const float frameDuration =
            1.0f / currentAnimation->framesPerSecond;

        while (frameTimer >= frameDuration) {
            frameTimer -= frameDuration;

            if (currentFrame + 1 < currentAnimation->frames.size()) {
                ++currentFrame;
            } else if (currentAnimation->looping) {
                currentFrame = 0;
            } else {
                // Hold the last frame when a non-looping animation ends.
                currentFrame = currentAnimation->frames.size() - 1;
                frameTimer = 0.0f;
                break;
            }
        }
    }

    bool SpriteRenderer::DrawAnimation(float x, float y) {
        if (!loaded ||
            currentAnimation == nullptr ||
            currentFrame >= currentAnimation->frames.size()) {
            return false;
        }

        return DrawSprite(
            currentAnimation->frames[currentFrame],
            x,
            y
        );
    }

    const atlas::TextureAtlas& SpriteRenderer::GetAtlas() const {
        return atlas;
    }

} // namespace renderer