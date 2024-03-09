#include "text.hpp"
#include "text_p.hpp"

#include <txtview/config.h>
#include <txtview/conversions.h>
#include <txtview/file.hpp>
#include <txtview/utils.hpp>

#include <harfbuzz/hb.h>
#include <cassert>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std::literals;

namespace txtview {

size_t GlyphCacheKey::HashCode() const {
    size_t seed = 0;
    HashCombineObj(seed, fontId);
    HashCombineObj(seed, glyphIndex);
    return seed;
}

template <typename T>
size_t TextAttrHashCode(const T& t) {
    size_t seed = 0;
    HashCombineObj(seed, t.lang);
    HashCombineObj(seed, t.script);
    HashCombineObj(seed, t.dir);
    HashCombineObj(seed, t.bold);
    HashCombineObj(seed, t.italic);
    return seed;
}

size_t RunBreakingAttr::HashCode() const {
    return TextAttrHashCode(*this);
}

LayoutCache::Private::Private(TypefaceLibrary& typefaceLib)
    : typefaceLibrary{ &typefaceLib }
    , hbBuffer{ hb_buffer_create() } //
{
    glyphAtlases.emplace_back();
}

LayoutCache::Private::~Private() {
    hb_buffer_destroy(hbBuffer);
}

const CachedGlyph& LayoutCache::Private::GetOrMakeGlyph(GlyphCacheKey key) {
    // Yes, we want the default constructing behavior of operator[]
    auto [iter, found] = glyphLut.try_emplace(key);
    auto& cg = iter->second;
    if (found)
        return cg;

    // TODO support multiple atlases
    cg.atlasId = 0;

    const ResidentFont& refo = typefaceLibrary->GetFont(key.fontId);
    FT_Face font = refo.ftFont;
    FT_Load_Glyph(font, key.glyphIndex, FT_LOAD_DEFAULT);
    FT_GlyphSlot glyph = font->glyph; // N.B. FT_Load_Glyph() modifies FT_Face::glyph, so we need to fetch this afterwards
    switch (glyph->format) {
        case FT_GLYPH_FORMAT_BITMAP:
            break;
        case FT_GLYPH_FORMAT_OUTLINE:
            FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);
            break;
        default:
            fprintf(stderr, "Error: cannot handle FT glyph format %d", glyph->format);
            break;
    }

    const FT_Bitmap& bitmap = glyph->bitmap;
    // TODO copy into atlas texture

    return cg;
}

LayoutCache::LayoutCache(TypefaceLibrary& typefaceLib)
    : _p{ new Private(typefaceLib) } {}

LayoutCache::~LayoutCache() {
    delete _p;
}

namespace {
void ItemizeParagraphForFormatting(const TextParagraph& src, LayoutBuffer& buf) {
    // TODO
    buf.runs.push_back(LayoutTextRun{
        .glyphs = {},
        .begin = src.begin,
        .end = src.end,
        .attr = {},
        .dir = HB_DIRECTION_LTR,
        .completelyShaped = false,
    });
}

void ItemizeParagraphForInvalidCodepoints(const TextParagraph& src, LayoutBuffer& buf, LayoutCache& cache) {
    auto& inputs = buf.runs;
    auto& outputs = buf.runsStash;
    size_t outputWriteIdx = 0;

    for (LayoutTextRun& run : inputs) {
        if (run.completelyShaped)
            continue;

        constexpr auto kInvalid = std::numeric_limits<uint32_t>::max();
        // A run of HB_CODEPOINT_INVALID that should be broken up for shaping
        uint32_t subrunBegin = kInvalid;
        uint32_t subrunEnd = kInvalid;
        for (size_t i = 0; i < run.glyphs.size(); ++i) {
            auto& [glyphInfo, _] = run.glyphs[i];

            if (glyphInfo.glyphIndex != HB_CODEPOINT_INVALID)
                continue;
            if (subrunBegin == kInvalid)
                subrunBegin = i;
            else {
                // TODO
                subrunEnd = i;
            }
        }
    }

    std::swap(inputs, outputs);
}

int ShapeParagraph(const TextParagraph& src, LayoutBuffer& buf, LayoutCache& cache) {
    hb_buffer_t* buffer = cache._p->hbBuffer;

    int numInvalidCodepoints = 0;

    for (LayoutTextRun& run : buf.runs) {
        if (run.completelyShaped)
            continue;

        hb_buffer_set_content_type(buffer, HB_BUFFER_CONTENT_TYPE_UNICODE);
        hb_buffer_add_utf8(
            buffer,
            src.string + src.begin,
            src.end - src.begin,
            run.begin,
            run.end - run.begin);

        hb_buffer_set_direction(buffer, HbDirectionUncompress(run.attr.dir));
        hb_buffer_set_script(buffer, HbScriptUncompress(run.attr.script));
        // TODO
        // hb_buffer_set_language(hb_buffer_t *buffer, hb_language_t language);

        hb_shape(cache._p->typefaceLibrary->GetFont(run.attr.font).hbFont, buffer, nullptr, 0);

        unsigned int glyphCount;
        auto hbGlyphInfos = hb_buffer_get_glyph_infos(buffer, &glyphCount);
        auto hbGlyphPositions = hb_buffer_get_glyph_positions(buffer, &glyphCount);
        run.completelyShaped = true;
        run.glyphs.resize(glyphCount);
        for (unsigned int j = 0; j < glyphCount; ++j) {
            auto& [gi, gp] = run.glyphs[j];

            auto const& hb_i = hbGlyphInfos[j];
            gi.glyphIndex = hb_i.codepoint;
            gi.cluster = hb_i.cluster;
            if (hb_i.codepoint == HB_CODEPOINT_INVALID) {
                run.completelyShaped = false;
                numInvalidCodepoints += 1;
            }

            auto const& hb_p = hbGlyphPositions[j];
            gp.xAdvance = hb_p.x_advance;
            gp.yAdvance = hb_p.y_advance;
            gp.xOff = hb_p.x_offset;
            gp.yOff = hb_p.y_offset;
        }

        run.dir = hb_buffer_get_direction(buffer);

        hb_buffer_clear_contents(buffer);
    }

    return numInvalidCodepoints;
}
} // namespace

void LayoutBuffer::ShapeWithParagraph(const TextParagraph& paragraph, LayoutCache& cache) {
    ItemizeParagraphForFormatting(paragraph, *this);
    while (true) {
        int numInvalidCodepoints = ShapeParagraph(paragraph, *this, cache);
        if (numInvalidCodepoints > 0)
            break;
        break; // TODO we just want to test the shaping codepath for now, the fallback itemization is sort of broken

        ItemizeParagraphForInvalidCodepoints(paragraph, *this, cache);
    }
}

void LayoutBuffer::GenerateLineBreaks(float boxWidth, float boxHeight) {
    // TODO
}

void LayoutBuffer::CollapseLineBreaks() {
    // TODO
}


RenderedBuffer LayoutBuffer::PlaceGlyphs(const TextParagraph& paragraph, LayoutCache& cache) const {
    RenderedBuffer res;

    // TOOD use line breaking data
    float penX = 0.0f;
    float penY = 0.0f;

    for (const LayoutTextRun& run : runs) {
        assert(run.completelyShaped);

        for (const auto& [glyphInfo, glyphPos] : run.glyphs) {
            const auto& glyphTex = cache._p->GetOrMakeGlyph(GlyphCacheKey{ run.attr.font, glyphInfo.glyphIndex });
            auto& quad = res.glyphs.emplace_back();
            quad.u0 = glyphTex.u0;
            quad.v0 = glyphTex.v0;
            quad.u1 = glyphTex.u1;
            quad.v1 = glyphTex.v1;
            quad.x0 = penX + static_cast<float>(glyphPos.xOff);
            quad.y0 = penY + static_cast<float>(glyphPos.yOff);
            quad.x1 = quad.x0 + static_cast<float>(glyphTex.width);
            quad.y1 = quad.y0 + static_cast<float>(glyphTex.height);
            penX += static_cast<float>(glyphPos.xAdvance);
        }
    }

    return res;
}

} // namespace txtview