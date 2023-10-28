#include "fontset.hpp"

#include <fontconfig/fontconfig.h>

namespace fs = std::filesystem;

namespace txtview {

namespace conv {
int FcFromStyle(FontStyle v) {
    switch (v) {
        using enum FontStyle;
        case Normal: return FC_SLANT_ROMAN;
        case Oblique: return FC_SLANT_OBLIQUE;
        case Italic: return FC_SLANT_ITALIC;
    }
    std::fputs("Error: invalid enum value", stderr);
    std::terminate();
}

int FcFromStretch(FontStretch v) {
    switch (v) {
        using enum FontStretch;
        case UltraCondensed: return FC_WIDTH_ULTRACONDENSED;
        case ExtraCondensed: return FC_WIDTH_EXTRACONDENSED;
        case Condensed: return FC_WIDTH_CONDENSED;
        case SemiCondensed: return FC_WIDTH_SEMICONDENSED;
        case Normal: return FC_WIDTH_NORMAL;
        case SemiExpanded: return FC_WIDTH_SEMIEXPANDED;
        case Expanded: return FC_WIDTH_EXPANDED;
        case ExtraExpanded: return FC_WIDTH_EXTRAEXPANDED;
        case UltraExpanded: return FC_WIDTH_ULTRAEXPANDED;
    }
    std::fputs("Error: invalid enum value", stderr);
    std::terminate();
}

int FcFromWeight(FontWeight v) {
    switch (v) {
        using enum FontWeight;
        case Thin: return FC_WEIGHT_THIN;
        case ExtraLight: return FC_WEIGHT_EXTRALIGHT;
        case Light: return FC_WEIGHT_LIGHT;
        case SemiLight: return FC_WEIGHT_SEMILIGHT;
        case Normal: return FC_WEIGHT_NORMAL;
        case Medium: return FC_WEIGHT_MEDIUM;
        case SemiBold: return FC_WEIGHT_SEMIBOLD;
        case Bold: return FC_WEIGHT_BOLD;
        case ExtraBold: return FC_WEIGHT_EXTRABOLD;
        case Black: return FC_WEIGHT_BLACK;
        case ExtraBlack: return FC_WEIGHT_EXTRABLACK;
    }
    std::fputs("Error: invalid enum value", stderr);
    std::terminate();
}

FontStyle FcToStyle(int v) {
    switch (v) {
        using enum FontStyle;
        case FC_SLANT_ROMAN: return Normal;
        case FC_SLANT_OBLIQUE: return Oblique;
        case FC_SLANT_ITALIC: return Italic;
    }
    std::fputs("Error: invalid enum value", stderr);
    std::terminate();
}

FontStretch FcToStretch(int v) {
    switch (v) {
        using enum FontStretch;
        case FC_WIDTH_ULTRACONDENSED: return UltraCondensed;
        case FC_WIDTH_EXTRACONDENSED: return ExtraCondensed;
        case FC_WIDTH_CONDENSED: return Condensed;
        case FC_WIDTH_SEMICONDENSED: return SemiCondensed;
        case FC_WIDTH_NORMAL: return Normal;
        case FC_WIDTH_SEMIEXPANDED: return SemiExpanded;
        case FC_WIDTH_EXPANDED: return Expanded;
        case FC_WIDTH_EXTRAEXPANDED: return ExtraExpanded;
        case FC_WIDTH_ULTRAEXPANDED: return UltraExpanded;
    }
    std::fputs("Error: invalid enum value", stderr);
    std::terminate();
}

FontWeight FcToWeight(int v) {
    switch (v) {
        using enum FontWeight;
        case FC_WEIGHT_THIN: return Thin;
        case FC_WEIGHT_EXTRALIGHT: return ExtraLight;
        case FC_WEIGHT_LIGHT: return Light;
        case FC_WEIGHT_SEMILIGHT: return SemiLight;
        case FC_WEIGHT_NORMAL: return Normal;
        case FC_WEIGHT_MEDIUM: return Medium;
        case FC_WEIGHT_SEMIBOLD: return SemiBold;
        case FC_WEIGHT_BOLD: return Bold;
        case FC_WEIGHT_EXTRABOLD: return ExtraBold;
        case FC_WEIGHT_BLACK: return Black;
        case FC_WEIGHT_EXTRABLACK: return ExtraBlack;
    }
    std::fputs("Error: invalid enum value", stderr);
    std::terminate();
}
} // namespace conv

class FontConfigResolver : public IFontResolver {
private:
    FcConfig* mFcConf;

public:
    FontConfigResolver() {
        FcInit();
        mFcConf = FcInitLoadConfigAndFonts();
    }

    ~FontConfigResolver() {
        FcFini();
    }

    std::vector<UnloadedFace> LocateMatchingFonts(const FontDescription& desc) override {
        FcPattern* pattern = FcPatternCreate();

        FcPatternAddBool(pattern, FC_OUTLINE, true);
        FcPatternAddBool(pattern, FC_SCALABLE, true);
        FcPatternAddInteger(pattern, FC_SLANT, conv::FcFromStyle(desc.style));
        FcPatternAddInteger(pattern, FC_WIDTH, conv::FcFromStretch(desc.stretch));
        FcPatternAddInteger(pattern, FC_WEIGHT, conv::FcFromWeight(desc.weight));
        for (const auto& familyName : desc.familyNames) {
            auto cstr = reinterpret_cast<const FcChar8*>(familyName.c_str());
            FcPatternAddString(pattern, FC_FAMILY, cstr);
        }

        FcConfigSubstitute(mFcConf, pattern, FcMatchPattern);
        FcDefaultSubstitute(pattern);

        std::vector<UnloadedFace> result;

        FcResult fcRes;
        FcFontSet* fontSet = FcFontSort(mFcConf, pattern, FcTrue, nullptr, &fcRes);
        if (fcRes == FcResultMatch) {
            for (int i = 0; i < fontSet->nfont; ++i) {
                FcPattern* font = fontSet->fonts[i];

                FcChar8* fontFileCstr = nullptr;
                if (FcPatternGetString(font, FC_FILE, 0, &fontFileCstr) != FcResultMatch)
                    continue;

                int fcSlant = FC_SLANT_ROMAN;
                int fcWidth = FC_WIDTH_NORMAL;
                int fcWeight = FC_WEIGHT_NORMAL;
                int ttc = -1;
                // TODO check for errors
                FcPatternGetInteger(font, FC_SLANT, 0, &fcSlant);
                FcPatternGetInteger(font, FC_WIDTH, 0, &fcWidth);
                FcPatternGetInteger(font, FC_WEIGHT, 0, &fcWeight);
                FcPatternGetInteger(font, FC_INDEX, 0, &ttc);

                result.push_back(UnloadedFace{
                    .file = fs::path(reinterpret_cast<const char*>(fontFileCstr)),
                    .style = conv::FcToStyle(fcSlant),
                    .stretch = conv::FcToStretch(fcWidth),
                    .weight = conv::FcToWeight(fcWeight),
                    .ttcIndex = ttc,
                });
            }
        }

        FcFontSetDestroy(fontSet);
        FcPatternDestroy(pattern);
        return result;
    }

    std::vector<UnloadedFace> LocateAllFonts() override {
        std::vector<UnloadedFace> result;
        // TODO
        return result;
    }
};

} // namespace txtview
