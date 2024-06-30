#pragma once

#include <txtview/fontset.hpp>
#include <txtview/measurement.hpp>

namespace txtview {

/*
struct TextSection {
    uint32_t srcBegin;
    uint32_t srcEnd;
    // If >0, indicates the child count
    // If ==-1, indicates that the subtree is trimmed
    int16_t childOffset;
    int16_t childCount;
};
*/

struct TextFormatting {
    FontSet font;
};

struct TextParagraph {
    const char* string = nullptr;
    uint32_t begin = 0;
    uint32_t end = 0;
    TextFormatting formatting;
    Length indent;
};

struct TextDocument {
    // std::vector<std::vector<TextSection>> groupings;
    std::vector<TextParagraph> paragraphs;

    TextDocument(std::string_view content);
};

/*
struct Loader {
    using PfnEmitParagraph = void(*)(const TextParagraph* para);

    void* p;
    void(*prevSection)(PfnEmitParagraph);
    void(*nextSection)(PfnEmitParagraph);
};

void LoadNextSection(const Loader& loader, TextDocument& doc);
void LoadPrevSection(const Loader& loader, TextDocument& doc);
*/

} // namespace txtview
