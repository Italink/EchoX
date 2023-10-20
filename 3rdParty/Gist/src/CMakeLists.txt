file(GLOB_RECURSE PROJECT_SOURCE FILES *.h *.cpp *.c *.hh)

add_library(Gist STATIC ${PROJECT_SOURCE})

target_include_directories (Gist PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (Gist PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libs/kiss_fft130)
target_compile_definitions (Gist PUBLIC -DUSE_KISS_FFT)

source_group (Source src)

