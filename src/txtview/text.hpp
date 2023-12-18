#pragma once

#include <txtview/fontset.hpp>
#include <txtview/measurement.hpp>
#include <txtview/text_document.hpp>

#include <harfbuzz/hb.h>

namespace txtview {

struct File;

struct TextParagraph;
struct ItemizedParagraph;
struct ShapedParagraph;

using Codepoint = uint32_t;
using GlyphIndex = uint32_t;

/******** Section: Itemization ********/

struct RunBreakingAttrFull {
    hb_language_t lang = HB_LANGUAGE_INVALID;
    ReFoIndex font;
    hb_direction_t dir = HB_DIRECTION_INVALID;
    hb_script_t script = HB_SCRIPT_INVALID;
    bool bold = false;
    bool italic = false;

    size_t HashCode() const;
    bool operator==(const RunBreakingAttrFull&) const = default;
};

struct RunBreakingAttrCompressed {
    hb_language_t lang;
    ReFoIndex font;
    // There are currently 166 scripts specified in hb_script_t, so a byte is sufficient
    uint8_t script = 0;
    // LTR, RTL, TTB, BTT, INVALID; 5 < 2^3
    uint8_t dir : 3 = 0;
    bool bold : 1 = false;
    bool italic : 1 = false;

    size_t HashCode() const;
    bool operator==(const RunBreakingAttrCompressed&) const = default;
};

using RunAttributes = RunBreakingAttrCompressed;
RunBreakingAttrCompressed CompressTextAttr(const RunBreakingAttrFull&);
RunBreakingAttrFull ExpandTextAttr(const RunBreakingAttrCompressed&);

// TODO this needs to be modified to "try shaping to do itemization" (i.e. glyph coverage of fonts)
// which probably means ItemizedParagraph needs to go away, and this whole system be hidden as an opaque cache object
// that sits between text formatting based run breaking and shaping
struct ScriptItem {
    uint32_t begin;
    uint32_t end;
    RunBreakingAttrCompressed attr;
};

struct ItemizedParagraph {
    TextParagraph* textParagraph;
    std::vector<ScriptItem> items;
};

/******** Section: Shaping ********/

// analogous to hb_glyph_info_t
struct GlyphInfo {
    // analogous to hb_glyph_info_t::codepoint after shaping
    GlyphIndex glyphIndex;
    // analogous to hb_glyph_info_t::cluster
    uint32_t cluster;
};

// analogous to hb_glyph_position_t
struct GlyphPosition {
    int32_t xAdvance, yAdvance;
    int32_t xOff, yOff;
};

struct ShapedRun {
    std::vector<GlyphInfo> glyphInfos;
    std::vector<GlyphPosition> glyphPositions;
    hb_direction_t dir;
};

struct ShapedParagraph {
    ItemizedParagraph* itemized;
    TextParagraph* source;
    std::vector<ShapedRun> runs;
};

/******** Section: Rasterization ********/

struct Quad {
    float x0, y0, x1, y1;
    float u0, v0, u1, v1;
    uint8_t r, g, b, a;
};

struct Canvas {
};

struct PlacementResult {
    float finalHeight;
};

/******** Section: Functions ********/

// Opaque object, handles caches, etc.
struct TextLibrary {
    struct Private;
    Private* _p;

    TextLibrary(TypefaceLibrary&);
    ~TextLibrary();

    // N.B. we use out parameters here to reuse std::vector, etc.'s allocations
    void ItemizeAndShape(TextParagraph& input, ItemizedParagraph& outItemization, ShapedParagraph& outShaping);
    /// Perform the first-pass run breaking on the paragraph input, handling formatting and script changes.
    void ItemizeParagraphInit(TextParagraph& input, ItemizedParagraph& output);
    /// Perform subsequent run breaking based on unresolved codepoints in existing ScriptItem's, as to pick the next font in the fallback chain.
    /// The expectation is that `ShapeParagraphOnePass()` and this function will be called alternatingly, until all codepoints have been resolved or that all fallback fonts have been exhausted.
    void ItemizeParagraphOnePass(ItemizedParagraph& obj);
    void ShapeParagraphOnePass(ItemizedParagraph& input, ShapedParagraph& output);

    void PlaceShapedRun(Canvas& canvas, const ShapedRun& run, float x0, float y0);
    void PlaceShapedParagraph(Canvas& canvas, const ShapedParagraph& para, PlacementResult* res, float boxX, float boxY, float boxWidth, float boxHeight);
    void PresentCanvas(const Canvas& canvas, std::vector<Quad>& out);
};

} // namespace txtview
