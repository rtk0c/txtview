#include "fontset.hpp"

#include <dwrite.h>
#include <wrl/client.h>
#include <cstdint>
#include <cstdio>
#include <memory>

// Define `HRESULT hr;` at top of function to use this
#define TRY_HRESULT(exp) \
    hr = exp;            \
    if (FAILED(hr)) continue

namespace fs = std::filesystem;
using namespace std::literals;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

template <typename T>
static void SafeRelease(T*& ptr) {
    if (ptr) {
        ptr->Release();
        ptr = nullptr;
    }
}

// https://gist.github.com/xebecnan/6d070c93fb69f40c3673
static std::string WcharToUTF8(const wchar_t* src, size_t srcLen) {
    if (!src)
        return {};
    if (srcLen == 0)
        srcLen = wcslen(src);

    int length = WideCharToMultiByte(CP_UTF8, 0, src, static_cast<int>(srcLen), nullptr, 0, nullptr, nullptr);
    std::string res(length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, src, static_cast<int>(srcLen), res.data(), length, nullptr, nullptr);
    return res;
}

namespace txtview {

FontStyle DwriteToStyle(DWRITE_FONT_STYLE v) {
    switch (v) {
        using enum FontStyle;
        case DWRITE_FONT_STYLE_NORMAL: return Normal;
        case DWRITE_FONT_STYLE_OBLIQUE: return Oblique;
        case DWRITE_FONT_STYLE_ITALIC: return Italic;
    }
    std::fputs("Error: invalid enum value", stderr);
    std::terminate();
}

FontStretch DwriteToStretch(DWRITE_FONT_STRETCH v) {
    switch (v) {
        using enum FontStretch;
        case DWRITE_FONT_STRETCH_UNDEFINED: break;
        case DWRITE_FONT_STRETCH_ULTRA_CONDENSED: return UltraCondensed;
        case DWRITE_FONT_STRETCH_EXTRA_CONDENSED: return ExtraCondensed;
        case DWRITE_FONT_STRETCH_CONDENSED: return Condensed;
        case DWRITE_FONT_STRETCH_SEMI_CONDENSED: return SemiCondensed;
        case DWRITE_FONT_STRETCH_NORMAL: return Normal;
        case DWRITE_FONT_STRETCH_SEMI_EXPANDED: return SemiExpanded;
        case DWRITE_FONT_STRETCH_EXPANDED: return Expanded;
        case DWRITE_FONT_STRETCH_EXTRA_EXPANDED: return ExtraExpanded;
        case DWRITE_FONT_STRETCH_ULTRA_EXPANDED: return UltraExpanded;
    }
    std::fputs("Error: invalid enum value", stderr);
    std::terminate();
}

FontWeight DwriteToWeight(DWRITE_FONT_WEIGHT v) {
    switch (v) {
        using enum FontWeight;
        case DWRITE_FONT_WEIGHT_THIN: return Thin;
        case DWRITE_FONT_WEIGHT_EXTRA_LIGHT: return ExtraLight;
        case DWRITE_FONT_WEIGHT_LIGHT: return Light;
        case DWRITE_FONT_WEIGHT_SEMI_LIGHT: return SemiLight;
        case DWRITE_FONT_WEIGHT_NORMAL: return Normal;
        case DWRITE_FONT_WEIGHT_MEDIUM: return Medium;
        case DWRITE_FONT_WEIGHT_DEMI_BOLD: return SemiBold;
        case DWRITE_FONT_WEIGHT_BOLD: return Bold;
        case DWRITE_FONT_WEIGHT_EXTRA_BOLD: return ExtraBold;
        case DWRITE_FONT_WEIGHT_BLACK: return Black;
        case DWRITE_FONT_WEIGHT_EXTRA_BLACK: return ExtraBlack;
    }
    std::fputs("Error: invalid enum value", stderr);
    std::terminate();
}

class DirectWriteResolver : public IFontResolver {
private:
    IDWriteFactory* mFactory = nullptr;

public:
    DirectWriteResolver() {
        HRESULT hr;

        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&mFactory));
        if (FAILED(hr))
            throw std::runtime_error("Failed to create IDWriteFactory");
    }

    ~DirectWriteResolver() override {
        SafeRelease(mFactory);
    }

    std::vector<UnloadedFace> LocateMatchingFaces(const FaceDescription& desc) override;
    std::vector<UnloadedFace> LocateAllFaces() override;
};

std::vector<UnloadedFace> DirectWriteResolver::LocateMatchingFaces(const FaceDescription& desc) {
    std::vector<UnloadedFace> result;
    // TODO
    return result;
}

std::vector<UnloadedFace> DirectWriteResolver::LocateAllFaces() {
    std::vector<UnloadedFace> result;

    HRESULT hr;

    ComPtr<IDWriteFontCollection> sysFontCollection;
    hr = mFactory->GetSystemFontCollection(&sysFontCollection);
    if (FAILED(hr))
        return result;

    UINT32 fontFamilyCnt = sysFontCollection->GetFontFamilyCount();
    for (UINT32 i = 0; i < fontFamilyCnt; ++i) {
        ComPtr<IDWriteFontFamily> fontFamily;
        TRY_HRESULT(sysFontCollection->GetFontFamily(i, &fontFamily));

        UINT32 fontCnt = fontFamily->GetFontCount();
        for (UINT32 j = 0; j < fontCnt; ++j) {
            ComPtr<IDWriteFont> font;
            TRY_HRESULT(fontFamily->GetFont(j, &font));

            ComPtr<IDWriteFontFace> fontFace;
            TRY_HRESULT(font->CreateFontFace(&fontFace));

            // Ignore type1 fonts since our font stack can't handle them gracefully
            if (fontFace->GetType() == DWRITE_FONT_FACE_TYPE_TYPE1)
                continue;

            UINT32 fontFilesCnt;
            fontFace->GetFiles(/* query number of files available */ &fontFilesCnt, nullptr);

            // https://stackoverflow.com/questions/41161152/when-can-an-idwritefontface-have-more-than-one-file
            // According to this, a IDWriteFontFace can never have more than 1 file except in the case of Type 1 fonts which we filter out above already
            if (fontFilesCnt > 1) {
                std::fputs("Warning: IDWriteFont reported more than 1 font file, skipping\n", stdout);
                continue;
            }

            ComPtr<IDWriteFontFile> fontFile;
            TRY_HRESULT(fontFace->GetFiles(/* provide number of font files to request */ &fontFilesCnt, &fontFile));

            ComPtr<IDWriteLocalFontFileLoader> localLoader;
            // TODO maybe we can fetch a TTF/OTF stream from the non-local font and feed it to Freetype directly as a memory blob?
            // Check if the loader is backed by a local file, in which case there is a file available for us to query
            {
                ComPtr<IDWriteFontFileLoader> loader;
                TRY_HRESULT(fontFile->GetLoader(&loader));
                TRY_HRESULT(loader.As<IDWriteLocalFontFileLoader>(&localLoader));
            }

            const void* refKey;
            UINT32 refKeySize;
            TRY_HRESULT(fontFile->GetReferenceKey(&refKey, &refKeySize));

            UINT32 filePathLen;
            // Gets file path length without null terminator
            TRY_HRESULT(localLoader->GetFilePathLengthFromKey(refKey, refKeySize, &filePathLen));

            // Add 1 to account for null terminator since GetFilePathFromKey wants it
            auto filePath = std::make_unique<WCHAR[]>(filePathLen + 1);
            TRY_HRESULT(localLoader->GetFilePathFromKey(refKey, refKeySize, filePath.get(), filePathLen + 1));

            // NOTE: "dw" here stands for "DirectWrite", not the "double word" seen usually in win32 programming
            BOOL dwSupported;
            DWRITE_FONT_FILE_TYPE dwFileType;
            DWRITE_FONT_FACE_TYPE dwFaceType;
            UINT32 numFaces;
            fontFile->Analyze(&dwSupported, &dwFileType, &dwFaceType, &numFaces);

            result.push_back({
                .file = fs::path(filePath.get(), filePath.get() + filePathLen),
                .style = DwriteToStyle(font->GetStyle()),
                .stretch = DwriteToStretch(font->GetStretch()),
                .weight = DwriteToWeight(font->GetWeight()),
            });
        }
    }

    return result;
}

} // namespace txtview
