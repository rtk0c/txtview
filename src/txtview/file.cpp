#include "file.hpp"

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace fs = std::filesystem;

namespace txtview {

File::File(const fs::path& path) {
    int fd = open(path.c_str(), O_RDONLY);
    struct stat filestat;

    if (fstat(fd, &filestat) != 0)
        throw std::runtime_error("fstat failed");

    void* data = mmap(NULL, filestat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED)
        throw std::runtime_error("mmap failed");

    content = reinterpret_cast<char*>(data);
    length = filestat.st_size;
}

File::File(File&& that) noexcept
    : content{ that.content }
    , length{ that.length } //
{
    that.content = nullptr;
}

File& File::operator=(File&& that) noexcept {
    if (content != nullptr)
        munmap(reinterpret_cast<void*>(content), length);
    content = std::exchange(that.content, nullptr);
    length = std::exchange(that.length, 0);
    return *this;
}

File::~File() {
    if (content != nullptr)
        munmap(reinterpret_cast<void*>(content), length);
}

} // namespace txtview
