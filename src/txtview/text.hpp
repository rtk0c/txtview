#pragma once

#include <txtview/fontset.hpp>

namespace txtview {

struct TextSection {
    uint32_t srcBegin;
    uint32_t srcEnd;
    // If >0, indicates the child count
    // If ==-1, indicates that the subtree is trimmed
    int32_t childCount;
};

struct TextDocument {
    std::vector<TextSection> sections;
};

using Codepoint = uint32_t;
using GlyphIndex = uint32_t;

struct GlyphCacheKey {
    ReFaIndex font;
    FontSize size;
    Codepoint codepoint;

    size_t HashCode() const;
    bool operator<=>(const GlyphCacheKey&) const = default;
};

struct GlyphCacheValue {
    GlyphIndex idx;
};

} // namespace txtview
