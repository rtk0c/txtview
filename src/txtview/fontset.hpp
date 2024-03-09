#pragma once

#include <harfbuzz/hb.h>
#include <filesystem>
#include <span>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace txtview {

/* The relationship between {Font|Face}Set and TypefaceStore:

To make a set theory ananlogy, TypefaceStore is the universe, and FontSet is a set in the universe.
Alternatively we can say a `FontSet` represents a subset of all the `ResidentFont`s stored in a `TypefaceStore`.

Here we also deviate from the the harfbuzz terminology, where `TypefaceStore` does not really mean
"typeface", but instead as a catch all term for "font related things".
*/

struct FaceSet;
struct FontSet;
struct TypefaceLibrary;

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

const char* StringifyFontStyle(FontStyle v);
const char* StringifyFontStretch(FontStretch v);
const char* StringifyFontWeight(FontWeight v);
int GetFontWeightConventionValue(FontWeight);

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

    std::string ToString() const;
};

/// "_Re_sident_Fa_ce _I_ndex"
using ReFaIndex = int;
/// "_Re_sident_Fo_nt _I_ndex"
using ReFoIndex = int;

struct ResidentFace {
    hb_face_t* hbFace = nullptr;
    ReFaIndex index = -1;
};

struct ResidentFont {
    hb_font_t* hbFont = nullptr;
    // Note the misnomer here, we call it a "font" to follow harfbuzz's terminology
    FT_Face ftFont = nullptr;
    ReFaIndex prototypeId = -1;
    ReFoIndex index = -1;
    FontSize size = 0;
};

struct FaceSet {
    std::vector<ReFaIndex> elements;
};

struct FontSet {
    std::vector<ReFoIndex> elements;
};

struct TypefaceLibrary {
    /// Constraint: universe[i].index == i
    std::vector<ResidentFace> universe;
    /// Constraint: fonts[i].index == i
    std::vector<ResidentFont> fonts;

    ResidentFont& GetFont(ReFoIndex fontId);
    ResidentFace& GetFace(ReFaIndex faceId);

    FontSet AddFonts(const FaceSet& faces, FontSize size);
    FaceSet AddFaces(std::span<const UnloadedFace> faces);
};

struct FaceDescription {
    std::span<const std::string> familyNames;
    FontStyle style = FontStyle::Normal;
    FontStretch stretch = FontStretch::Normal;
    FontWeight weight = FontWeight::Normal;
};

class IFontResolver {
public:
    virtual ~IFontResolver() = default;
    virtual std::vector<UnloadedFace> LocateMatchingFaces(const FaceDescription& desc) = 0;
    virtual std::vector<UnloadedFace> LocateAllFaces() = 0;
};

IFontResolver& GetCanonicalFontResolver();

} // namespace txtview
