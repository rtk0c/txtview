// Sample API usage file

namespace my_utils { ... }

int main() {
    std::string file = my_utils::LoadFile("path/to/fff.txt");

    txtview::FontEnumerator fe;
    auto fontset = fe.resolve({"Helvetica Neue", "Noto Sans CJK", "sans-serif"});

    txtview::TextFormatting fmt;
    fmt.font = &fontset;
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
    p.writing_direction = txtview::WD_LeftToRight; // RightToLeft, TopToBottom, BottomToTop
    p.stacking_direction = txtview::SD_TopToBottom; // RightToLeft, LeftToRight, RightToLeft

    // Coordinates: top-left (0,0) x positive right, y positive down
    txtview::Canvas canvas;
    // When writing direction is horizontal, set width; height must be 0 (rep. infinite)
    // When writing direction is vertical, set height; width must be 0 (rep. infinite)
    canvas.width = 200;
    canvas.height = 0;

    txtview::Typesetter engine;
    txtview::TextParagraphIntermediates p_int;
    engine.typeset(p, p_int, canvas);
    
    auto quads = engine.generate_glyph_quads(p, p_int);
    ...
}
