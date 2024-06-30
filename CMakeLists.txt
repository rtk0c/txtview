cmake_minimum_required(VERSION 3.13)
project(txtview LANGUAGES CXX)

# Load PkgConfig
find_package(PkgConfig REQUIRED)

# Section: platform libraries
if(UNIX AND NOT APPLE)
  find_package(Fontconfig REQUIRED)
endif()

# Section: vcpkg libraries
# If user doesn't want to use vcpkg, simply not provide -DCMAKE_TOOLCHAIN_FILE=/path/vcpkg when configuring.
# These imports should be compatible with each project's own CMake/PkgConfig files (typically seen shipped in Linux distros)
find_package(Freetype REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)

file(GLOB_RECURSE txtview_SRC_FILES
    src/*.c
    src/*.cpp
)
add_executable(txtview ${txtview_SRC_FILES})

target_include_directories(txtview PRIVATE src)
target_precompile_headers(txtview
PRIVATE
    <algorithm>
    <array>
    <cstddef>
    <cstdint>
    <filesystem>
    <format>
    <iostream>
    <memory>
    <span>
    <string>
    <string_view>
    <unordered_map>
    <utility>
    <variant>
    <vector>
)

target_link_libraries(txtview
PRIVATE
    Freetype::Freetype
    harfbuzz::harfbuzz
    ${SDL2_LIBRARIES}
    PkgConfig::tomlplusplus
)
if(WIN32)
    target_link_libraries(txtview
    PRIVATE
        Dwrite
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(txtview
    PRIVATE
        Fontconfig::Fontconfig
    )
endif()

set_target_properties(
    txtview
PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS OFF
)
