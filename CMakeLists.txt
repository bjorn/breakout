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

find_package(SDL3 REQUIRED CONFIG REQUIRED)

target_link_libraries(breakout PRIVATE SDL3::SDL3)
target_compile_options(breakout PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wpedantic>
)
