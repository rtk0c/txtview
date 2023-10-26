#pragma once

#include "fontset.hpp"

namespace txtview {

struct TextSection {
	size_t srcBegin;
	size_t srcEnd;
	uint32_t childCount;
};

struct TextDocument {
	std::vector<TextSection> sections;
};

using GlyphIndex = size_t;

struct Glyph {
	GlyphIndex idx;
};

struct RenderData {
	std::vector<SDL_Vertex> vertices;
};

void GenerateRenderData(RenderData& rd, std::span<const > text);

} // namespace txtview
