#include "fontset.hpp"
#include "fontset_p.hpp"

namespace txtview {

class CompositeFontResolver : public IFontResolver {
private:
	std::unique_ptr<IFontResoler> a, b;

public:

};

std::unique_ptr<IFontResolver> MakeDefaultResolver() {
}

} // namespace txtview

//#if _WIN32
//#include "fontset-directwrite.inl"
//#endif
//#if __APPLE__
//#include "fontset-coretext.inl"
//#endif
#if __linux__
#include "fonset-fontconfig.inl"
#endif
