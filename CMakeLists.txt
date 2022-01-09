cmake_minimum_required(VERSION 3.16)

project(adt_modder)

add_executable(adt_modder src/main.cpp src/adt.c)

target_include_directories(adt_modder PUBLIC inc)
target_compile_features(adt_modder PUBLIC cxx_std_17)
target_link_libraries(adt_modder PUBLIC fmt)

add_subdirectory(fmt)
