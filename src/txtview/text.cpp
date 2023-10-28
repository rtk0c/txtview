#include "text.hpp"

#include <txtview/utils.hpp>

namespace txtview {

size_t GlyphCacheKey::HashCode() const {
	size_t seed = 0;
	HashCombineObj(seed, font);
	HashCombineObj(seed, size);
	HashCombineObj(seed, codepoint);
	return seed;
}

} // namespace txtview