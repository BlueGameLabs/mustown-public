#ifndef MUSTOWN_ATLASREGION_H
#define MUSTOWN_ATLASREGION_H

namespace renderer::atlas {

    class AtlasRegion {
    public:
        AtlasRegion() = default;

        AtlasRegion(float x, float y, float width, float height);

        float GetX() const;
        float GetY() const;
        float GetWidth() const;
        float GetHeight() const;

    private:
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
    };

} // namespace renderer::atlas

#endif // MUSTOWN_ATLASREGION_H