#include "text_document.hpp"

#include <txtview/measurement.hpp>

using namespace std::literals;

namespace txtview {

TextDocument::TextDocument(const File& file) {
    std::string_view content(file.content, file.length);
    // TODO this doesn't handle if a paragraph is at the beginning of a file
    constexpr auto kPrefix = "\n\u3000\u3000"sv;
    constexpr auto kSuffix = "\n"sv;

    size_t cursor = 0;
    while (true) {
        auto begin = content.find(kPrefix, cursor);
        if (begin == std::string_view::npos)
            break;
        // We don't want the prefix in our paragraph content
        begin += kPrefix.length();
        auto end = content.find(kSuffix, begin);
        if (end == std::string_view::npos) {
            end = content.length();
            break;
        }

#ifdef TXTVIEW_TEXT_DOCUMENT_DEBUG
        printf("found paragraph: [%zu,%zu)\n", begin, end);
        fwrite(file.content + begin, 1, end - begin, stdout);
        putc('\n', stdout);
#endif

        paragraphs.push_back({
            .begin = static_cast<uint32_t>(begin),
            .end = static_cast<uint32_t>(end),
            .indent = Length{ 2, LengthUnit::em },
        });
        cursor = end;
    }
}

} // namespace txtview
