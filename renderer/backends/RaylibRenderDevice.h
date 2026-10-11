#ifndef MUSTOWN_RAYLIBRENDERDEVICE_H
#define MUSTOWN_RAYLIBRENDERDEVICE_H

#include "../RenderDevice.h"

#include <raylib.h>
#include <unordered_map>

namespace renderer::backends {

    class RaylibRenderDevice final : public renderer::RenderDevice {
    public:
        RaylibRenderDevice() = default;
        ~RaylibRenderDevice() override;

        RaylibRenderDevice(const RaylibRenderDevice&) = delete;
        RaylibRenderDevice& operator=(const RaylibRenderDevice&) = delete;

        bool LoadTexture(
            const std::string& path,
            renderer::TextureHandle& outTexture
        ) override;

        void UnloadTexture(
            renderer::TextureHandle texture
        ) override;

        void DrawTextureRegion(
            renderer::TextureHandle texture,
            const renderer::atlas::AtlasRegion& source,
            float x,
            float y
        ) override;

    private:
        renderer::TextureHandle nextHandle = 1;
        std::unordered_map<renderer::TextureHandle, ::Texture2D> textures;
    };

} // namespace renderer::backends

#endif // MUSTOWN_RAYLIBRENDERDEVICE_H