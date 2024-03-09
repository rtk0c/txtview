#pragma once

#include <txtview/fontset.hpp>
#include <txtview/measurement.hpp>
#include <txtview/text_document.hpp>

#include <harfbuzz/hb.h>

namespace txtview {

using GlyphIndex = uint32_t;

struct TextStyles {
    FontSet fontset;
};

struct RunBreakingAttr {
    hb_language_t lang = HB_LANGUAGE_INVALID;
    ReFoIndex font;
    hb_direction_t dir = HB_DIRECTION_INVALID;
    hb_script_t script = HB_SCRIPT_INVALID;
    bool bold = false;
    bool italic = false;

    size_t HashCode() const;
    bool operator==(const RunBreakingAttr&) const = default;
};

// analogous to hb_glyph_info_t
// We have this separate type because hb_glyph_info_t contains some internal fields, this way we save a bit of memory
struct GlyphInfo {
    // analogous to hb_glyph_info_t::codepoint after shaping
    GlyphIndex glyphIndex;
    // analogous to hb_glyph_info_t::cluster
    uint32_t cluster;
};

// analogous to hb_glyph_position_t
struct GlyphPosition {
    int32_t xAdvance = 0;
    int32_t yAdvance = 0;
    int32_t xOff = 0;
    int32_t yOff = 0;
};

struct LayoutTextRun {
    std::vector<std::pair<GlyphInfo, GlyphPosition>> glyphs;
    uint32_t begin = 0;
    uint32_t end = 0;
    RunBreakingAttr attr;
    hb_direction_t dir = HB_DIRECTION_LTR;
    bool completelyShaped = false;
};

struct LayoutCache {
    struct Private;
    Private* _p;

    explicit LayoutCache(TypefaceLibrary&);
    ~LayoutCache();
};

struct Quad {
    float x0, y0, x1, y1;
    float u0, v0, u1, v1;
    uint8_t r, g, b, a;
};

struct RenderedBuffer {
    std::vector<Quad> glyphs;
};

/// Results for text shaping
struct LayoutBuffer {
    std::vector<LayoutTextRun> runs;
    std::vector<LayoutTextRun> runsStash;

    /// Perform initial shaping that turns the stream of Unicode codepoitns into a list of runs each with the same text attributes.
    /// These runs logically exist on a single, infinitely long line.
    void ShapeWithParagraph(const TextParagraph& paragraph, LayoutCache& cache);
    /// Place runs on in a box by performing line breaking. Splits a single run into multiple if it needs to span across
    /// a line boundary (since each LayoutTextRun spans only a continuous, 1d space).
    /// This operation is done in place.
    void GenerateLineBreaks(float boxWidth, float boxHeight);
    /// Collapse text runs that are broken up during GenerateLineBreaks() into one again. Effectively undoes that function.
    /// Call this function, and then GenerateLineBreaks() again if the same text needs to be redisplayed in a different box.
    /// THis operation is done in place.
    void CollapseLineBreaks();
    /// Generate renderable glyphs from the layout runs.
    RenderedBuffer PlaceGlyphs(const TextParagraph& paragraph, LayoutCache& cache) const;
};

} // namespace txtview
