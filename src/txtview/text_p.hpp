#pragma once
#include "text.hpp"

#include <txtview/utils.hpp>

#include <SDL2/SDL.h>

namespace txtview {

struct GlyphAtlas {
    SDL_Texture* texture;
};

struct GlyphCacheKey {
    ReFoIndex fontId;
    GlyphIndex glyphIndex;

    size_t HashCode() const;
    bool operator==(const GlyphCacheKey&) const = default;
};

struct CachedGlyph {
    uint32_t atlasId;
    // Size (in pixels) of this glyph
    int width, height;
    // Texture coordinates in the atlas
    float u0, v0;
    float u1, v1;
};

struct LayoutCache::Private {
    std::vector<GlyphAtlas> glyphAtlases;
    HashMap<GlyphCacheKey, CachedGlyph> glyphLut;
    TypefaceLibrary* typefaceLibrary;
    // The common, reused buffer for all shaping operations
    // N.B. we assume an object TextLibrary is only used from one thread ever
    hb_buffer_t* hbBuffer;

    explicit Private(TypefaceLibrary& typefaceLib);
    ~Private();

    const CachedGlyph& GetOrMakeGlyph(GlyphCacheKey key);
};

} // namespace txtview
