// Sample API usage file

int main() {
    std::string file = my_load_file("path/to/fff.txt");

    txtview::TypefaceCache tfcache;

    // RasterMethod or: SDF
    // - in case of something like Apple Emoji that uses CBIX/CBLC or SBIX tables, GlyphCache will automatically choose a new atlas in bitmap mode
    //   - TODO provide a hook for a user provided jpeg/png/tiff loader functions for SBIX
    // - TODO how do we support COLR/CPAL?
    // - TODO how _tf_ do we support SVG?
    // RasterLib or: DirectWrite, CoreText, Freetype
    txtview::GlyphCache gcache(txtview::RasterMethod::Bitmap, txtview::RasterLib::PlatformDefault);

    // or: DirectWrite, CoreText, FontConfig
    // each may be available depending on the current platform
    txtview::TypefaceEnumerator te(txtview::FaceEnumLib::PlatformDefault);
    // Load all the given typefaces by name, and create a fallback chain object
    // Think of faceset as a line of `font-family: xxx, yyy, zzz;` in CSS
    auto faceset = te.resolve(tfcache, {"Helvetica Neue", "Noto Sans CJK", "sans-serif"});
    // "Helvetica Neue" here would be fetched from `tfcache`, instead of querying & loading the font again
    auto another_faceset = te.resolve(tfcache, {"Helvetica Neue", "Twemoji"});

    txtview::TextFormatting fmt;
    // N.B. this is called font because we're specializing the typefaces in `faceset` with a size parameter, making it a font
    fmt.font = &faceset;
    fmt.font_size = 12;
    fmt.bold = false;
    fmt.italic = false;

    txtview::TextParagraph p;
    p.buf = txtview::StringBuffer(file.c_str(), file.size());
    p.elements = {
        txtview::TextElement{ 0, file.size(), fmt },
        // More elements as needed...
        // Each different style must be a different element
    };
    p.writing_direction = txtview::WritingDirection::LeftToRight; // RightToLeft, TopToBottom, BottomToTop
    p.stacking_direction = txtview::StackingDirection::TopToBottom; // RightToLeft, LeftToRight, RightToLeft

    // Divide up the stream of TextElement's based on glyph coverage.
    // For example, if the paragraph contains "ABCD一二三四09876", the single TextElement above will probably be divided into 3
    // ("ABCD" for Helvetica Neue, "一二三四" for Noto Sans CJK, "09876" for Helvetica Neue again)
    // in accordance with which actual font will be used to render the text runs.
    // In harfbuzz terms, this pass subdivides each text run until each run contains no invalid codepoints with respect to its font
    txtview::preshape_paragraph(p);

    // Coordinates: top-left (0,0) x positive right, y positive down
    txtview::Canvas canvas;
    // When writing direction is horizontal, set width; height must be 0 (rep. infinite)
    // When writing direction is vertical, set height; width must be 0 (rep. infinite)
    canvas.width = 200;
    canvas.height = 0;

    // Subdivide the text runs again (if needed) to fit into the given canvas box.
    // At this stage, `p` should contain every rendered glyph's OpenType index and position
    txtview::typeset_paragraph(p, canvas);

    /*
    // Alternatively, if you are doing something like PostScript generation, where rasterization is the concern, you can get the glyphs out without rasterization (and without creating a `txtview::GlyphCache` at the beginning).
    auto glyphs - txtview::prepare_glyphs_only(p);
    */

    // Actually rasterize each glyph
    auto glyphs = txtview::prepare_glyphs_for_drawing(p, gcache);

    // render quads onto the screen
    // something like this, all MyXxx types here are demonstrational

    std::unordered_map<const txtview::TextureAtlas*, MyDrawList> dls;
    MyDrawList* curr_dl = nullptr;

    for (const auto& g : glyphs) {
        if (!curr_dl || curr_dl->get_texture() != g.atlas.texture_id) {
            auto [iter, _] = dls.try_emplace(g.atlas, MyDrawList());
            curr_dl = &iter->second;
            curr_dl->set_translation(100.0f, 30.0f); // or something
            curr_dl->set_texture(g.atlas.texture_id);
            switch (g.atlas.type) {
                using enum txtview::RasterMethod;
                // Need this if we requested SDF at all
                // We did not
                //case SDF: curr_dl->set_shader(sdf_shader); break;
                case Bitmap: curr_dl->set_shader(bitmap_shader); break;
            }
        }

        // Loads translation and current texture from MyTransforms State
        curr_dl->add_quad(g.x0, g.y0, g.x1, g.y1,
                          g.u0, g.v0, g.u1, g.v1,
                          g.r, g.g, g.b, g.a);
    }

    for (auto&& [_, dl] : dls) {
        my_app.submit_draw_list(std::move(dl));
    }
}
