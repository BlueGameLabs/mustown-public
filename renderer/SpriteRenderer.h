#ifndef MUSTOWN_SPRITERENDERER_H
#define MUSTOWN_SPRITERENDERER_H

#include <string>
#include <unordered_map>
#include <vector>

#include "RenderDevice.h"
#include "atlas/TextureAtlas.h"

namespace renderer {

    class SpriteRenderer {
    public:
        explicit SpriteRenderer(RenderDevice& device);
        ~SpriteRenderer();

        SpriteRenderer(const SpriteRenderer&) = delete;
        SpriteRenderer& operator=(const SpriteRenderer&) = delete;

        bool Load(
            const std::string& texturePath,
            const std::string& atlasPath
        );

        void Unload();

        bool IsLoaded() const;

        bool DrawSprite(
            const std::string& name,
            float x,
            float y
        );

        bool AddAnimation(
            const std::string& name,
            const std::vector<std::string>& frames,
            float framesPerSecond,
            bool looping = true
        );

        bool PlayAnimation(const std::string& name);

        void StopAnimation();

        void Update(float deltaTime);

        bool DrawAnimation(float x, float y);

        const atlas::TextureAtlas& GetAtlas() const;

    private:
        struct Animation {
            std::vector<std::string> frames;
            float framesPerSecond = 1.0f;
            bool looping = true;
        };

        RenderDevice& device;
        atlas::TextureAtlas atlas;

        TextureHandle texture = 0;
        bool loaded = false;

        std::unordered_map<std::string, Animation> animations;

        const Animation* currentAnimation = nullptr;
        std::string currentAnimationName;

        std::size_t currentFrame = 0;
        float frameTimer = 0.0f;

        void ResetAnimationState();
    };

} // namespace renderer

#endif // MUSTOWN_SPRITERENDERER_H