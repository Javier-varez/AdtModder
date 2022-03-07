cmake_minimum_required(VERSION 3.16)

project(adt_modder)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_executable(adt_modder
    src/main.cpp
    src/adt_modder.cpp
    src/adt_modder/replace_prop_op.cpp
    src/adt_modder/randomize_prop_op.cpp
    src/adt_modder/zero_out_prop_op.cpp
    src/adt_modder/delete_prop.cpp
    src/fileio.cpp
    src/adt.c)

target_include_directories(adt_modder PUBLIC
    include)

target_compile_features(adt_modder PUBLIC
    cxx_std_20)

target_link_libraries(adt_modder PUBLIC
    fmt
    argparse
    Ditto
    nlohmann_json)

add_subdirectory(fmt)
add_subdirectory(argparse)
add_subdirectory(Ditto)
add_subdirectory(json)
