#ifndef MUSTOWN_TEXTUREATLAS_H
#define MUSTOWN_TEXTUREATLAS_H

#include <cstddef>
#include <string>
#include <unordered_map>

#include "AtlasRegion.h"

namespace renderer::atlas {

    class TextureAtlas {
    public:
        static constexpr int CELL_SIZE = 32;

        bool LoadFromFile(const std::string& path);

        bool AddRegion(
            const std::string& name,
            const AtlasRegion& region
        );

        const AtlasRegion* GetRegion(
            const std::string& name
        ) const;

        bool HasRegion(const std::string& name) const;

        bool RemoveRegion(const std::string& name);

        void Clear();

        std::size_t GetRegionCount() const;

    private:
        std::unordered_map<std::string, AtlasRegion> regions;
    };

} // namespace renderer::atlas

#endif // MUSTOWN_TEXTUREATLAS_H