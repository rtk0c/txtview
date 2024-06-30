#include <SDL2/SDL.h>
#include <fstream>
#include <sstream>

#include <txtview/fontset.hpp>
#include <txtview/text.hpp>

namespace fs = std::filesystem;
using namespace std::literals;

std::string_view ParseLongArgValue(std::string_view arg, std::string_view name) {
    size_t prefixLen = 2 /*--*/ + name.length() + 1 /*=*/;
    if (arg.length() < prefixLen)
        return {};
    if (arg[0] != '-' || arg[1] != '-')
        return {};
    arg.remove_prefix(2);
    if (!arg.starts_with(name))
        return {};
    if (arg[prefixLen - 1] != '=')
        return {};
    arg.remove_prefix(prefixLen);
    return arg;
}

std::string_view SubstringAndStrim(std::string_view src, size_t begin, size_t end) {
    while (begin <= end && std::isspace(src[begin]))
        ++begin;
    while (end >= begin && std::isspace(src[end]))
        --end;
    return src.substr(begin, end - begin);
}

void SubcommandResolveFont(std::string_view argFamilies, std::string_view argStyles) {
    using namespace txtview;

    std::vector<std::string> familyNames;
    size_t lastElm = 0;
    for (size_t i = 0; i < argFamilies.length(); ++i) {
        if (argFamilies[i] == ',') {
            familyNames.push_back(std::string(SubstringAndStrim(argFamilies, lastElm, i)));
            lastElm = i + 1;
        }
    }
    familyNames.push_back(std::string(SubstringAndStrim(argFamilies, lastElm, argFamilies.size())));

    FaceDescription desc;
    desc.familyNames = familyNames;

    auto faces = GetCanonicalFontResolver().LocateMatchingFaces(desc);

    printf("--x-resolve-font: querying %.*s\n", static_cast<int>(argFamilies.length()), argFamilies.data());
    printf("--x-resolve-font: got %zu faces\n", faces.size());
    for (const auto& face : faces) {
        printf("\t%s\n", face.ToString().c_str());
    }
}

int main(int argc, char** argv) {
    fs::path configPath;
    fs::path inputPath;

    int nthPositionalArg = 0;
    bool positionalOnly = false;
    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);

        if (positionalOnly)
            goto handlePositionalArg;

        if (auto val = ParseLongArgValue(arg, "config"sv); !val.empty()) {
            configPath = fs::path(val);
        }
        if (arg == "--x-resolve-font") {
            std::string_view familyNames(argv[i + 1]);
            std::string_view styles(argv[i + 2]);
            SubcommandResolveFont(familyNames, styles);
            i += 2;
            continue;
        }
        if (arg == "--x-exit") {
            return 0;
        }
        if (arg == "--") {
            positionalOnly = true;
            continue;
        }

    handlePositionalArg:
        switch (nthPositionalArg++) {
            case 0:
                inputPath = fs::path(arg);
                break;
            default:
                fputs(std::format("Unexpected positional argument '{}'\n", arg).c_str(), stderr);
                return -1;
        }
    }

    txtview::TypefaceLibrary libTypeface;
    txtview::LayoutCache libText(libTypeface);
    std::ifstream inputFileStream(inputPath);
    std::stringstream inputFileBuf;
    inputFileBuf << inputFileStream.rdbuf();

    txtview::TextDocument doc(inputFileBuf.view());

    struct ProcessedParagraph {
        // txtview::ParagraphItemization itemization;
        // txtview::ParagraphShaping shaping;
    };
    std::vector<ProcessedParagraph> paragraphs;
//    txtview::Canvas canvas;

    float currX = 0.0f;
    float currY = 0.0f;
    float pageWidth = 100.0f;

    paragraphs.resize(doc.paragraphs.size());
    for (size_t i = 0; i < paragraphs.size(); ++i) {
        auto& text = doc.paragraphs[i];
        auto& proc = paragraphs[i];
        // txtview::PlacementResult res;
        // libText.ItemizeAndShape(text, proc.itemization, proc.shaping);
        // libText.PlaceShapedParagraph(canvas, proc.shaping, currX, currY, pageWidth, 0.0f);
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fputs(std::format("SDL initialization failed: {}\n", SDL_GetError()).c_str(), stderr);
        return -1;
    }

    auto window = SDL_CreateWindow("txtview", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 612, 791, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        fputs(std::format("SDL window creation failed: {}\n", SDL_GetError()).c_str(), stderr);
        std::cerr << "SDL : " << SDL_GetError() << '\n';
        return -1;
    }

    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        fputs(std::format("SDL renderer creation failed: {}\n", SDL_GetError()).c_str(), stderr);
        return -1;
    }

    int windowWidth, windowHeight;
    bool quit = false;
    bool refreshDisplay = true;
    while (!quit) {
        // TODO fetch multiple events at once, when such is available?
        SDL_Event event;
        SDL_WaitEvent(&event);

        switch (event.type) {
            case SDL_WINDOWEVENT_RESIZED:
                SDL_GetWindowSize(window, &windowWidth, &windowHeight);
                break;
            case SDL_QUIT:
                quit = true;
                break;
        }

        if (refreshDisplay) {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(renderer);
            // TODO
            SDL_RenderPresent(renderer);
            refreshDisplay = false;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
