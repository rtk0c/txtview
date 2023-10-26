#pragma once

#include <txtview/fontset.hpp>

namespace txtview {

struct StyleElement {
	enum PatternType {
		PT_Regex,
		PT_Instrinsic;
	};

	/**** Config ****/
	/// Case: starts with "regex:", the tail is treated as a regex for matching elements
	/// Case: starts with "preset:", the tail must be a predefined NAME, implemented by instrinstics in C++ code
	std::string pattern;
	FontDescription fontFamily;
	PatternType patternType;

	/**** Computed (at runtime, transient) ****/
	struct {
		// TODO compiled pattern pcre? boost::regex?
		const Font* font = nullptr;
	} computed;
}

struct StylePreset {
	std::vector<StyleElement> elements;
};

struct Config {
	StylePreset styles;
};

} // namespace txtview
