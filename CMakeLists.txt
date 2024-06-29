cmake_minimum_required(VERSION 3.13)
project(txtview LANGUAGES CXX)

if(UNIX AND NOT APPLE)
  find_package(Fontconfig REQUIRED)
endif()
find_package(Freetype REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)

file(GLOB_RECURSE txtview_SRC_FILES
    src/*.c src/*.cpp src/*.cpp2
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
    tomlplusplus::tomlplusplus
)
if(UNIX AND NOT APPLE)
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
