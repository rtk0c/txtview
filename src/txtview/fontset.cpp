#include "fontset.hpp"

#include <txtview/utils.hpp>

#if _WIN32
#include "fontset-directwrite.inl"
#elif __APPLE__
#include "fontset-coretext.inl"
#elif __linux__
#include "fontset-fontconfig.inl"
#endif

namespace txtview {

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

FontSet TypefaceStore::SolidifyFaces(const FaceSet &faces) {
    FontSet result;
    // TODO
    return result;
}

FaceSet TypefaceStore::AddFaces(std::span<const UnloadedFace> faces) {
#if _WIN32
    using PathCStr = const wchar_t*;
#define GET_PATH_CSTR(p) p.string().c_str()
#else
    using PathCStr = const char*;
#define GET_PATH_CSTR(p) p.c_str()
#endif

    FaceSet result;
    HashMap<PathCStr, hb_blob_t*> blobs;
    auto findOrLoadBlob = [&](const fs::path& file) {
        PathCStr key = GET_PATH_CSTR(file);

        if (auto iter = blobs.find(key); iter != blobs.end())
            return iter->second;

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
#error "Platform not supported"
#endif
    return *instance;
}

} // namespace txtview
