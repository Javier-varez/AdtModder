cmake_minimum_required(VERSION 3.16)

project(adt_modder)

add_executable(adt_modder
    src/main.cpp
    src/fileio.cpp
    src/adt.c)

target_include_directories(adt_modder PUBLIC
    include)

target_compile_features(adt_modder PUBLIC
    cxx_std_20)

target_link_libraries(adt_modder PUBLIC
    fmt
    argparse
    Ditto)

add_subdirectory(fmt)
add_subdirectory(argparse)
add_subdirectory(Ditto)
