#pragma once

namespace txtview {

enum class LengthUnit {
    pt,
    em,
};

struct Length {
    int32_t value = 0;
    LengthUnit unit = LengthUnit::pt;
};

} // namespace txtview