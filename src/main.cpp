#include <SDL2/SDL.h>

namespace fs = std::filesystem;
using namespace std::literals;

struct OutputDrawList {
};

void PaintDrawList(SDL_Renderer* renderer, const OutputDrawList& drawList) {
}

std::string_view ParseLongArgValue(std::string_view arg, std::string_view name) {
    size_t prefixLen = 2 /*--*/ + name.length() + 1 /*=*/;
    if (arg.length() < prefixLen)
        return {};
    if (arg[0] != '-' || arg[1] != '-')
        return {};
    if (!arg.remove_prefix(2).starts_with(name))
        return {};
    if (arg[prefixLen - 1] != '=')
        return {};
    return arg.remove_prefix(prefixLen);
}

int main(int argc, char** argv)  {
    fs::path configFile;
    fs::path inputFile;

    int nthPositionalArg = 0;
    bool positionalOnly = false;
    for (int i = 1; i < argc; ++i) {
        std::string_view arg(argv[i]);

        if (positionalOnly)
            goto handlePositionalArg;

        if (auto val = ParseLongArgValue("config"sv); !val.empty()) {
            configFile = fs::path(val);
        }
        if (arg == "--"sv) {
            positionalOnly = true;
            continue;
        }

    handlePositionalArg:
        switch (nthPositionalArg++) {
            case 0:
                inputFile = fs::path(arg);
                break;
            default:
                fputs(std::format("Unexpected positional argument '{}'\n", arg).c_str(), stderr);
                return -1;
        }
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

    OutputDrawList drawList;

    bool quit = false;
    bool refreshDisplay = false;
    while (!quit) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
        }

        if (refreshDisplay) {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(renderer);
            PaintDrawList(renderer, drawList);
            SDL_RenderPresent(renderer);
            refreshDisplay = false;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
