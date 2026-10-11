#include "TextureAtlas.h"

#include <fstream>
#include <sstream>
#include <utility>

namespace renderer::atlas {

    bool TextureAtlas::LoadFromFile(const std::string& path) {
        std::ifstream file(path);

        if (!file.is_open()) {
            return false;
        }

        std::unordered_map<std::string, AtlasRegion> loadedRegions;
        std::string line;

        while (std::getline(file, line)) {
            std::istringstream stream(line);

            std::string name;
            int column;
            int row;
            int width;
            int height;

            if (!(stream >> name)) {
                continue;
            }

            // Ignore comment lines.
            if (name[0] == '#') {
                continue;
            }

            if (!(stream >> column >> row >> width >> height)) {
                return false;
            }

            // Coordinates are grid-based; dimensions are in cells.
            if (column < 0 || row < 0 ||
                width <= 0 || height <= 0) {
                return false;
            }

            const float x = static_cast<float>(column * CELL_SIZE);
            const float y = static_cast<float>(row * CELL_SIZE);
            const float pixelWidth =
                static_cast<float>(width * CELL_SIZE);
            const float pixelHeight =
                static_cast<float>(height * CELL_SIZE);

            const auto result = loadedRegions.emplace(
                name,
                AtlasRegion(x, y, pixelWidth, pixelHeight)
            );

            // Duplicate sprite names are invalid.
            if (!result.second) {
                return false;
            }
        }

        if (file.bad() || loadedRegions.empty()) {
            return false;
        }

        // Replace the current atlas only after successful parsing.
        regions = std::move(loadedRegions);

        return true;
    }

    bool TextureAtlas::AddRegion(
        const std::string& name,
        const AtlasRegion& region
    ) {
        if (name.empty() ||
            region.GetX() < 0 ||
            region.GetY() < 0 ||
            region.GetWidth() <= 0 ||
            region.GetHeight() <= 0) {
            return false;
        }

        return regions.emplace(name, region).second;
    }

    const AtlasRegion* TextureAtlas::GetRegion(
        const std::string& name
    ) const {
        const auto it = regions.find(name);

        if (it == regions.end()) {
            return nullptr;
        }

        return &it->second;
    }

    bool TextureAtlas::HasRegion(const std::string& name) const {
        return regions.find(name) != regions.end();
    }

    bool TextureAtlas::RemoveRegion(const std::string& name) {
        return regions.erase(name) > 0;
    }

    void TextureAtlas::Clear() {
        regions.clear();
    }

    std::size_t TextureAtlas::GetRegionCount() const {
        return regions.size();
    }

} // namespace renderer::atlas