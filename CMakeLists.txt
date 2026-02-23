cmake_minimum_required(VERSION 3.21)

project(Breakout LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(breakout
    main.cpp
    p_engine.cpp
    ptypes.cpp
    base.cpp
)

# WebAssembly (Emscripten) vs native SDL3
if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    message(STATUS "Configuring for WebAssembly (Emscripten)")

    # Emit an HTML launcher for convenience
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    target_link_options(breakout PRIVATE
        # Use SDL3 from Emscripten port
        "-sUSE_SDL=3"
        "-sALLOW_MEMORY_GROWTH=1"
        "$<$<CONFIG:Debug>:-sASSERTIONS=1>"
        # Embed the data directory into the binary (no external .data file)
        "--embed-file" "${CMAKE_SOURCE_DIR}/data@/data"
    )
else()
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(breakout PRIVATE SDL3::SDL3)
endif()

target_compile_options(breakout PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wpedantic>
)
