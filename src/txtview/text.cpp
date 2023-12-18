#include "text.hpp"
#include "text_p.hpp"

#include <txtview/config.h>
#include <txtview/conversions.h>
#include <txtview/file.hpp>
#include <txtview/utils.hpp>

using namespace std::literals;

namespace txtview {

TextLibrary::Private::Private(TypefaceLibrary& typefaceLib)
    : typefaceLibrary{ &typefaceLib }
    , hbBuffer{ hb_buffer_create() } //
{
}

TextLibrary::Private::~Private() {
    hb_buffer_destroy(hbBuffer);
}

} // namespace txtview

namespace txtview {

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

size_t RunBreakingAttrFull::HashCode() const { return TextAttrHashCode(*this); }
size_t RunBreakingAttrCompressed::HashCode() const { return TextAttrHashCode(*this); }

RunBreakingAttrCompressed CompressTextAttr(const RunBreakingAttrFull& inp) {
    RunBreakingAttrCompressed res;
    res.lang = inp.lang;
    res.font = inp.font;
    res.script = HbScriptCompress(inp.script);
    res.dir = HbDirectionCompress(inp.dir);
    res.bold = inp.bold;
    res.italic = inp.italic;
    return res;
}

RunBreakingAttrFull ExpandTextAttr(const RunBreakingAttrCompressed& inp) {
    RunBreakingAttrFull res;
    res.lang = inp.lang;
    res.font = inp.font;
    res.script = HbScriptUncompress(inp.script);
    res.dir = HbDirectionUncompress(inp.dir);
    res.bold = inp.bold;
    res.italic = inp.italic;
    return res;
}

size_t GlyphCacheKey::HashCode() const {
    size_t seed = 0;
    HashCombineObj(seed, font);
    HashCombineObj(seed, size);
    HashCombineObj(seed, codepoint);
    return seed;
}

const ResidentFont& GetOrLoadFont(ReFaIndex face) {
}

TextLibrary::TextLibrary(TypefaceLibrary& typefaceLib)
    : _p{ new Private(typefaceLib) } {
}

TextLibrary::~TextLibrary() {
    delete _p;
}

void TextLibrary::ItemizeAndShape(TextParagraph& input, ItemizedParagraph& outItemization, ShapedParagraph& outShaping) {
    ItemizeParagraphInit(input, outItemization);
    ShapeParagraphOnePass(outItemization, outShaping);
}

void TextLibrary::ItemizeParagraphInit(TextParagraph& input, ItemizedParagraph& output) {
    output.textParagraph = &input;
    output.items.clear();
    // TODO
    output.items.push_back(ScriptItem{
        .begin = input.begin,
        .end = input.end,
        .attr = RunBreakingAttrCompressed{},
    });
}

void TextLibrary::ItemizeParagraphOnePass(ItemizedParagraph& obj) {
    // TODO
}

void TextLibrary::ShapeParagraphOnePass(ItemizedParagraph& input, ShapedParagraph& output) {
    if (input.items.empty())
        return;

    hb_buffer_t* buffer = _p->hbBuffer;

    output.runs.clear();
    output.runs.resize(input.items.size());
    for (size_t i = 0; i < input.items.size(); ++i) {
        auto const& item = input.items[i];
        auto& run = output.runs[i];

        hb_buffer_set_content_type(buffer, HB_BUFFER_CONTENT_TYPE_UNICODE);
        hb_buffer_add_utf8(
            buffer,
            input.textParagraph->string + input.textParagraph->begin,
            input.textParagraph->end - input.textParagraph->begin,
            input.items[0].begin,
            input.items[0].end - input.items[0].begin);

        hb_buffer_set_direction(buffer, HbDirectionUncompress(item.attr.dir));
        hb_buffer_set_script(buffer, HbScriptUncompress(item.attr.script));
        // TODO
        // hb_buffer_set_language(hb_buffer_t *buffer, hb_language_t language);

        hb_shape(_p->typefaceLibrary->GetFont(item.attr.font).hbFont, buffer, nullptr, 0);

        unsigned int count;
        auto hbGlyphInfos = hb_buffer_get_glyph_infos(buffer, &count);
        auto hbGlyphPositions = hb_buffer_get_glyph_positions(buffer, &count);
        run.glyphInfos.clear();
        run.glyphPositions.clear();
        for (unsigned int j = 0; j < count; ++j) {
            auto& d = run.glyphInfos[j];
            auto const& s = hbGlyphInfos[j];
            d.glyphIndex = s.codepoint;
            d.cluster = s.cluster;
        }
        for (unsigned int j = 0; j < count; ++j) {
            auto& d = run.glyphPositions[j];
            auto const& s = hbGlyphPositions[j];
            d.xAdvance = s.x_advance;
            d.yAdvance = s.y_advance;
            d.xOff = s.x_offset;
            d.yOff = s.y_offset;
        }

        run.dir = hb_buffer_get_direction(buffer);

        hb_buffer_clear_contents(buffer);
    }
}

void TextLibrary::PlaceShapedRun(Canvas& canvas, const ShapedRun& run, float x0, float y0) {
    if (run.dir == HB_DIRECTION_BTT || run.dir == HB_DIRECTION_TTB) {
        // TODO
        fputs("Vertical text unsupported", stderr);
        return;
    }


}

void TextLibrary::PlaceShapedParagraph(Canvas& canvas, const ShapedParagraph& para, float boxX, float boxY, float boxWidth, float boxHeight) {
    float currX = boxX;
    float currY = boxY;
    for (const auto& run : para.runs) {
        PlaceShapedRun(canvas, run, currX, currY);
    }
}

void TextLibrary::PresentCanvas(const Canvas& canvas, std::vector<Quad>& out) {
}

} // namespace txtview