#include "fontset.hpp"

#include <txtview/config.h>
#include <txtview/utils.hpp>

#include <fontconfig/fontconfig.h>

namespace fs = std::filesystem;
using namespace std::literals;

namespace txtview {

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

const char* StringifyFcSlant(int v) {
    return StringifyFontStyle(FcToStyle(v));
}

const char* StringifyFcWidth(int v) {
    return StringifyFontStretch(FcToStretch(v));
}

const char* StringifyFcWeight(int v) {
    return StringifyFontWeight(FcToWeight(v));
}

class FontConfigResolver : public IFontResolver {
private:
    FcConfig* mFcConf;
    UnloadedFace* mExclusionList = nullptr;
    int mExclusionCount = 0;

public:
    FontConfigResolver() {
        FcInit();
        mFcConf = FcInitLoadConfigAndFonts();
    }

    ~FontConfigResolver() {
        FcFini();
    }

    void ComputeExclusionList() {
        std::vector<std::string> family1{ "sans-serif"s };
        std::vector<std::string> family2{ "serif"s };
        auto set1 = ComputeMatchingFaces({ .familyNames = family1 }, nullptr, 0);
        auto set2 = ComputeMatchingFaces({ .familyNames = family2 }, nullptr, 0);

        auto it1 = set1.rbegin();
        auto it2 = set2.rbegin();
        while (it1 != set1.rend() && it2 != set2.rend()) {
            if (it1->file != it2->file)
                break;
            ++it1;
            ++it2;
        }

        size_t tailLen1 = std::distance(set1.rbegin(), it1) + 1;
        // Must be the same, no need to check
        // size_t tailLen2 = std::distance(set2.rbegin(), it2) + 1;

        mExclusionList = new UnloadedFace[tailLen1];
        mExclusionCount = tailLen1;

        for (int i = 0; i < tailLen1; ++i) {
            mExclusionList[i] = std::move(*(it1 - i));
        }

#ifdef TXTVIEW_DEBUG_FONTCONFIG
        printf("exclusion list:\n");
        for (int i = 0; i < mExclusionCount; ++i) {
            printf("  %s\n", mExclusionList[i].file.c_str());
        }
#endif
    }

    std::vector<UnloadedFace> ComputeMatchingFaces(const FaceDescription& desc, const UnloadedFace* exclusionList, int exclusionCount) {
#ifdef TXTVIEW_DEBUG_FONTCONFIG
        std::fputs("querying: ", stdout);
        for (size_t i = 0; i < desc.familyNames.size(); i++) {
            std::fputs(desc.familyNames[i].c_str(), stdout);
            bool isLast = i + 1 == desc.familyNames.size();
            if (!isLast)
                std::fputs(", ", stdout);
        }
        std::fputs("\n", stdout);
        std::printf("          style=%s, stretch=%s, weight=%s\n",
            StringifyFontStyle(desc.style), StringifyFontStretch(desc.stretch), StringifyFontWeight(desc.weight));
#endif

        ScopedPtr<FcPattern> pattern(FcPatternCreate(), FcPatternDestroy);

        FcPatternAddBool(pattern.get(), FC_OUTLINE, true);
        FcPatternAddBool(pattern.get(), FC_SCALABLE, true);
        FcPatternAddInteger(pattern.get(), FC_SLANT, FcFromStyle(desc.style));
        FcPatternAddInteger(pattern.get(), FC_WIDTH, FcFromStretch(desc.stretch));
        FcPatternAddInteger(pattern.get(), FC_WEIGHT, FcFromWeight(desc.weight));
        for (const auto& familyName : desc.familyNames) {
            auto cstr = reinterpret_cast<const FcChar8*>(familyName.c_str());
            FcPatternAddString(pattern.get(), FC_FAMILY, cstr);
        }

        FcConfigSubstitute(mFcConf, pattern.get(), FcMatchPattern);
        FcDefaultSubstitute(pattern.get());

        std::vector<UnloadedFace> result;

        FcResult fcRes;
        ScopedPtr<FcFontSet> fontSet(FcFontSort(mFcConf, pattern.get(), FcTrue, nullptr, &fcRes), FcFontSetDestroy);
        if (fcRes != FcResultMatch)
            return result;

        int usefulFontsCnt = fontSet->nfont;
        for (int i = 1; i <= exclusionCount; ++i) {
            auto excludedPath = exclusionList[exclusionCount - i].file.c_str();

            int fontSetIdx = fontSet->nfont - i;
            if (fontSetIdx < 0)
                // This really shouldn't happen logically, but if it somehow does, let's not read into uninitialized memory
                break;
            FcChar8* filePath;
            FcPatternGetString(fontSet->fonts[fontSetIdx], FC_FILE, 0, &filePath);

            if (strcmp(reinterpret_cast<const char*>(filePath), excludedPath) != 0)
                break;
#ifdef TXTVIEW_DEBUG_FONTCONFIG
            printf("ignoring font %s\n", excludedPath);
#endif
            --usefulFontsCnt;
        }

        for (int i = 0; i < usefulFontsCnt; ++i) {
            FcPattern* font = fontSet->fonts[i];

            FcChar8* fontFileCstr = nullptr;
            if (FcPatternGetString(font, FC_FILE, 0, &fontFileCstr) != FcResultMatch)
                continue;

            int fcSlant = FC_SLANT_ROMAN;
            int fcWidth = FC_WIDTH_NORMAL;
            int fcWeight = FC_WEIGHT_NORMAL;
            int ttc = -1;

            int tmp;
            if (FcPatternGetInteger(font, FC_SLANT, 0, &tmp) == FcResultMatch) fcSlant = tmp;
            if (FcPatternGetInteger(font, FC_WIDTH, 0, &tmp) == FcResultMatch) fcWidth = tmp;
            if (FcPatternGetInteger(font, FC_WEIGHT, 0, &tmp) == FcResultMatch) fcWeight = tmp;
            if (FcPatternGetInteger(font, FC_INDEX, 0, &tmp) == FcResultMatch) ttc = tmp;

#ifdef TXTVIEW_DEBUG_FONTCONFIG
            FcChar8* name;
            FcPatternGetString(font, FC_FAMILY, 0, &name);
            std::printf("got font: %s '%s', slant=%s, width=%s, weight=%s, ttcIdx=%d\n",
                fontFileCstr, name, StringifyFcSlant(fcSlant), StringifyFcWidth(fcWidth), StringifyFcWeight(fcWeight), ttc);
#endif

            result.push_back(UnloadedFace{
                .file = fs::path(reinterpret_cast<const char*>(fontFileCstr)),
                .style = FcToStyle(fcSlant),
                .stretch = FcToStretch(fcWidth),
                .weight = FcToWeight(fcWeight),
                .ttcIndex = ttc,
            });
        }

        return result;
    }

    std::vector<UnloadedFace> LocateMatchingFaces(const FaceDescription& desc) override {
#ifdef TXTVIEW_FONTCONFIG_DEFAULT_EXCLUSIONS
        if (mExclusionList == nullptr)
            ComputeExclusionList();
#endif
        return ComputeMatchingFaces(desc, mExclusionList, mExclusionCount);
    }

    std::vector<UnloadedFace> LocateAllFaces() override {
        std::vector<UnloadedFace> result;
        // TODO
        return result;
    }
};

} // namespace txtview
