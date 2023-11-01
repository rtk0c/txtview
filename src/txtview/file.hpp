#pragma once

#include <filesystem>

namespace txtview {

struct File {
    char* content;
    size_t length;

    File(const std::filesystem::path&);
    File(File&&) noexcept;
    File& operator=(File&&) noexcept;
    ~File();
};

} // namespace txtview