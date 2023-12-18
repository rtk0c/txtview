#pragma once
#include "text.hpp"

#include <SDL2/SDL.h>

namespace txtview {

struct GlyphAtlas {
    SDL_Texture* texture;
};

struct GlyphCacheKey {
    ReFaIndex font;
    FontSize size;
    Codepoint codepoint;

    size_t HashCode() const;
    bool operator==(const GlyphCacheKey&) const = default;
};

struct GlyphCacheValue {
    uint32_t atlasId;
    // Texture coordinates in the atlas
    float u0, v0;
    float u1, v1;
};

struct GlyphCache {
    std::vector<GlyphAtlas> atlases;
    std::unordered_map<GlyphCacheKey, GlyphCacheValue> cache;
};

struct TextLibrary::Private {
    TypefaceLibrary* typefaceLibrary;
    // The common, reused buffer for all shaping operations
    // N.B. we assume an object TextLibrary is only used from one thread ever
    hb_buffer_t* hbBuffer;
    GlyphCache fontCache;

    explicit Private(TypefaceLibrary& typefaceLib);
    ~Private();
};

} // namespace txtview
