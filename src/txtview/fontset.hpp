#pragma once

namespace txtview {

enum class FontStyle {
   Normal,
   Oblique,
   Italic,
};

enum class FontStretch {
    UltraCondensed,
    ExtraCondensed,
    Condensed,
    SemiCondensed,
    Normal,
    SemiExpanded,
    Expanded,
    ExtraExpanded,
    UltraExpanded,
};

enum class FontWeight {
    Thin = 100,
    ExtraLight = 200,
    Light = 300,
    SemiLight = 350,
    Normal = 400,
    Medium = 500,
    SemiBold = 600,
    Bold = 700,
    ExtraBold = 800,
    Black = 900,
    ExtraBlack = 950,
};

struct FontDescription {
    std::span<std::string> familyNames;
    FontStyle style = FontStyle::Normal;
    FontStretch stretch = FontStretch::Normal;
    FontWeight weight = FontWeight::Normal;
};

struct Font {
};

class IFontResolver {
public:
    ~IFontResolver() = default;
    virtual std::vector<Font> LocateMatchingFonts(std::span<const FontDescription&> descs) = 0;
    virtual std::vector<Font> LocateAllFonts() = 0;
};

std::unique_ptr<IFontResolver> MakeDefaultResolver();

} // namespace txtview

