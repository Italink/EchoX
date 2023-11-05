cmake_minimum_required (VERSION 3.12)
project (Gist VERSION 1.0.7)

set (CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE PROJECT_SOURCE FILES *.h *.cpp *.c *.hh)

add_library(Gist STATIC ${PROJECT_SOURCE})
target_link_libraries(Gist PUBLIC PFFFT)
target_include_directories(Gist PUBLIC src)

source_group (Source src)

set (CMAKE_SUPPRESS_REGENERATION true)

