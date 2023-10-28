#pragma once

#include <freetype/freetype.h>
#include <harfbuzz/hb.h>

#include <filesystem>
#include <span>

namespace txtview {

/* The relationship between {Font|Face}Set and TypefaceStore:

To make a set theory ananlogy, TypefaceStore is the universe, and FontSet is a set in the universe.
Alternatively we can say a `FontSet` represents a subset of all the `ResidentFont`s stored in a `TypefaceStore`.

Here we also deviate from the the harfbuzz terminology, where `TypefaceStore` does not really mean
"typeface", but instead as a catch all term for "font related things".
*/

struct FaceSet;
struct FontSet;
struct TypefaceStore;


enum class FontStyle : uint8_t {
    Normal,
    Oblique,
    Italic,
};

enum class FontStretch : uint8_t {
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

enum class FontWeight : uint8_t {
    Thin,
    ExtraLight,
    Light,
    SemiLight,
    Normal,
    Medium,
    SemiBold,
    Bold,
    ExtraBold,
    Black,
    ExtraBlack,
};
int GetFontWeightConventionValue(FontWeight);

struct FontDescription {
    std::span<const std::string> familyNames;
    FontStyle style = FontStyle::Normal;
    FontStretch stretch = FontStretch::Normal;
    FontWeight weight = FontWeight::Normal;
};

/* Note we use harfbuzz terminology here:

a "face" means some "font file" sitting somewhere, that's not inherently tied to a size
a "font" means a [type]face tied to a specific size, and rasterization options like antialiasing.
*/

/// Unit: CSS pt, i.e. on a 96dpi space
using FontSize = int;
constexpr FontSize kNormalFontSize = 14;

struct UnloadedFace {
    std::filesystem::path file;
    FontStyle style = FontStyle::Normal;
    FontStretch stretch = FontStretch::Normal;
    FontWeight weight = FontWeight::Normal;
    int ttcIndex = -1;
};

/// "_Re_sident_Fa_ce _I_ndex"
using ReFaIndex = int;
/// "_Re_sident_Fo_nt _I_ndex"
using ReFoIndex = int;

struct ResidentFace {
    hb_face_t* hbFace;
    ReFaIndex index = -1;
};

struct ResidentFont {
    hb_font_t* hbFont;
    // Note the misnomer here, we call it a "font" to follow harfbuzz's terminology
    FT_Face* fbFont;
    ReFaIndex prototypeId;
    FontSize size;
};


struct FaceSet {
    std::vector<ReFaIndex> elements;
};

struct FontSet {
    std::vector<ReFoIndex> elements;
};

struct TypefaceStore {
    /// Constraint: fonts[i].index == i
    std::vector<ResidentFace> universe;
    std::vector<ResidentFont> fonts;

    FontSet SolidifyFaces(const FaceSet& faces);
    FaceSet AddFaces(std::span<const UnloadedFace> faces);
};


class IFontResolver {
public:
    ~IFontResolver() = default;
    virtual std::vector<UnloadedFace> LocateMatchingFonts(const FontDescription& desc) = 0;
    virtual std::vector<UnloadedFace> LocateAllFonts() = 0;
};

IFontResolver& GetCanonicalFontResolver();

} // namespace txtview
