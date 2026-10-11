#include "AtlasRegion.h"

namespace renderer::atlas {

    AtlasRegion::AtlasRegion(
        float x,
        float y,
        float width,
        float height
    )
        : x(x),
          y(y),
          width(width),
          height(height) {}

    float AtlasRegion::GetX() const {
        return x;
    }

    float AtlasRegion::GetY() const {
        return y;
    }

    float AtlasRegion::GetWidth() const {
        return width;
    }

    float AtlasRegion::GetHeight() const {
        return height;
    }

} // namespace renderer::atlas