#include "RaylibRenderDevice.h"

namespace renderer::backends {

    RaylibRenderDevice::~RaylibRenderDevice() {
        for (const auto& [handle, texture] : textures) {
            ::UnloadTexture(texture);
        }
    }

    bool RaylibRenderDevice::LoadTexture(
        const std::string& path,
        renderer::TextureHandle& outTexture
    ) {
        ::Texture2D texture = ::LoadTexture(path.c_str());

        if (texture.id == 0) {
            outTexture = 0;
            return false;
        }

        const renderer::TextureHandle handle = nextHandle++;

        textures.emplace(handle, texture);
        outTexture = handle;

        return true;
    }

    void RaylibRenderDevice::UnloadTexture(
        renderer::TextureHandle handle
    ) {
        const auto it = textures.find(handle);

        if (it == textures.end()) {
            return;
        }

        ::UnloadTexture(it->second);
        textures.erase(it);
    }

    void RaylibRenderDevice::DrawTextureRegion(
        renderer::TextureHandle handle,
        const renderer::atlas::AtlasRegion& source,
        float x,
        float y
    ) {
        const auto it = textures.find(handle);

        if (it == textures.end()) {
            return;
        }

        const Rectangle sourceRect = {
            source.GetX(),
            source.GetY(),
            source.GetWidth(),
            source.GetHeight()
        };

        const Vector2 position = {x, y};

        ::DrawTextureRec(
            it->second,
            sourceRect,
            position,
            WHITE
        );
    }

} // namespace renderer::backends