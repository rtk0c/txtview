#include "fontset.hpp"

#include <txtview/utils.hpp>

#if _WIN32
#    include "fontset-directwrite.inl"
#endif
#if __APPLE__
#    include "fontset-coretext.inl"
#endif
#if __linux__
#    include "fontset-fontconfig.inl"
#endif

namespace txtview {

const char* StringifyFontStyle(FontStyle v) {
    switch (v) {
        using enum FontStyle;
        case Normal: return "Normal";
        case Oblique: return "Oblique";
        case Italic: return "Italic";
    };
    return "";
}

const char* StringifyFontStretch(FontStretch v) {
    switch (v) {
        using enum FontStretch;
        case UltraCondensed: return "UltraCondensed";
        case ExtraCondensed: return "ExtraCondensed";
        case Condensed: return "Condensed";
        case SemiCondensed: return "SemiCondensed";
        case Normal: return "Normal";
        case SemiExpanded: return "SemiExpanded";
        case Expanded: return "Expanded";
        case ExtraExpanded: return "ExtraExpanded";
        case UltraExpanded: return "UltraExpanded";
    };
    return "";
}

const char* StringifyFontWeight(FontWeight v) {
    switch (v) {
        using enum FontWeight;
        case Thin: return "Thin";
        case ExtraLight: return "ExtraLight";
        case Light: return "Light";
        case SemiLight: return "SemiLight";
        case Normal: return "Normal";
        case Medium: return "Medium";
        case SemiBold: return "SemiBold";
        case Bold: return "Bold";
        case ExtraBold: return "ExtraBold";
        case Black: return "Black";
        case ExtraBlack: return "ExtraBlack";
    };
    return "";
}

int GetFontWeightConventionValue(FontWeight v) {
    switch (v) {
        using enum FontWeight;
        case Thin: return 100;
        case ExtraLight: return 200;
        case Light: return 300;
        case SemiLight: return 350;
        case Normal: return 400;
        case Medium: return 500;
        case SemiBold: return 600;
        case Bold: return 700;
        case ExtraBold: return 800;
        case Black: return 900;
        case ExtraBlack: return 950;
    }
    std::fputs("Error: invalid enum value", stderr);
    std::terminate();
}

std::string UnloadedFace::ToString() const {
    return std::format("{} style={}, stretch={}, weight={}, ttcIndex={}",
        file.string(), StringifyFontStyle(style), StringifyFontStretch(stretch), StringifyFontWeight(weight), ttcIndex);
}

FontSet TypefaceStore::SolidifyFaces(const FaceSet& faces) {
    FontSet result;
    // TODO
    return result;
}

FaceSet TypefaceStore::AddFaces(std::span<const UnloadedFace> faces) {
    FaceSet result;
    std::unordered_map<const typename fs::path::value_type*, hb_blob_t*> blobs;
    auto findOrLoadBlob = [&](const fs::path& path) {
        auto key = path.c_str();

        if (auto iter = blobs.find(key); iter != blobs.end())
            return iter->second;

#if _WIN32
        auto pathStr = path.string();
        auto utf8path = pathStr.c_str();
#else
        auto utf8path = path.c_str();
#endif
        auto [iter, _] = blobs.try_emplace(key, hb_blob_create_from_file(key));
        return iter->second;
    };

    for (const auto& uf : faces) {
        ResidentFace rf;
        rf.index = static_cast<int>(this->fonts.size());
        rf.hbFace = hb_face_create(findOrLoadBlob(uf.file), uf.ttcIndex);

        result.elements.push_back(rf.index);
        universe.push_back(std::move(rf));
    }

    for (const auto& [_, blob] : blobs) {
        hb_blob_destroy(blob);
    }

    return result;
}

IFontResolver& GetCanonicalFontResolver() {
    static std::unique_ptr<IFontResolver> instance
#if _WIN32
        = std::make_unique<DirectWriteResolver>();
#elif __APPLE__
        = std::make_unique<CoreTextResolver>();
#elif __linux__
        = std::make_unique<FontConfigResolver>();
#else
#    error "Platform not supported"
#endif
    return *instance;
}

} // namespace txtview
