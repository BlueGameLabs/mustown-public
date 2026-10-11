#ifndef MUSTOWN_RENDERDEVICE_H
#define MUSTOWN_RENDERDEVICE_H

#include <cstdint>
#include <string>

#include "atlas/AtlasRegion.h"

namespace renderer {

    using TextureHandle = std::uint32_t;

    class RenderDevice {
    public:
        virtual ~RenderDevice() = default;

        virtual bool LoadTexture(
            const std::string& path,
            TextureHandle& outTexture
        ) = 0;

        virtual void UnloadTexture(
            TextureHandle texture
        ) = 0;

        virtual void DrawTextureRegion(
            TextureHandle texture,
            const atlas::AtlasRegion& source,
            float x,
            float y
        ) = 0;
    };

} // namespace renderer

#endif // MUSTOWN_RENDERDEVICE_H