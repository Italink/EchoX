cmake_minimum_required(VERSION 2.8)
project(PRETTY_FAST_FFT)

# smaller library size?
option(USE_TYPE_FLOAT  "activate single precision 'float'?" ON)
option(USE_TYPE_DOUBLE "activate 'double' precision float?" ON)

# architecture/optimization options
option(USE_SIMD        "use SIMD (SSE/AVX/NEON/ALTIVEC) CPU features? - " ON)
option(DISABLE_SIMD_AVX "disable AVX CPU features? - " OFF)
option(USE_SIMD_NEON   "force using NEON on ARM? (requires USE_SIMD)" OFF)
option(USE_SCALAR_VECT "use 4-element vector scalar operations (if no other SIMD)" ON)

# what to install?
option(INSTALL_PFFFT      "install pffft to CMAKE_INSTALL_PREFIX?" OFF)
option(INSTALL_PFDSP      "install pfdsp to CMAKE_INSTALL_PREFIX?" OFF)
option(INSTALL_PFFASTCONV "install pffastconv to CMAKE_INSTALL_PREFIX?" OFF)


option(USE_DEBUG_ASAN  "use GCC's address sanitizer?" OFF)

option(DISABLE_LINK_WITH_M "Disables linking with m library to build with clangCL from MSVC" OFF)

# C90 requires the gcc extensions for function attributes like always_inline
# C99 provides the function attributes: no gcc extensions required
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# populate what to install
set(INSTALL_TARGETS "")
set(INSTALL_HEADERS "")


if ( (NOT USE_TYPE_FLOAT) AND (NOT USE_TYPE_DOUBLE) )
  message(FATAL_ERROR "activate at least one of USE_TYPE_FLOAT or USE_TYPE_DOUBLE")
endif()


if (USE_DEBUG_ASAN)
  set(ASANLIB "asan")
else()
  set(ASANLIB "")
endif()


########################################################################
# select the release build type by default to get optimization flags
########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)

if ( CMAKE_C_COMPILER_ID MATCHES "MSVC" )
  # using Visual Studio C++
  message(STATUS "INFO: detected MSVC: will not link math lib m")
  set(MATHLIB "")

  add_definitions("/D_CRT_SECURE_NO_WARNINGS")

  set(MSVC_DISABLED_WARNINGS_LIST
      "C4996"
  )

else()
  if(DISABLE_LINK_WITH_M)
  else()
    message(STATUS "INFO: detected NO MSVC: ${CMAKE_C_COMPILER_ID}: will link math lib m")
    set(MATHLIB "m")
  endif()
endif()

set( SIMD_FLOAT_HDRS simd/pf_float.h simd/pf_sse1_float.h simd/pf_altivec_float.h simd/pf_neon_float.h simd/pf_scalar_float.h )
set( SIMD_DOUBLE_HDRS simd/pf_double.h simd/pf_avx_double.h simd/pf_scalar_double.h )

if (USE_TYPE_FLOAT)
  set( FLOAT_SOURCES pffft.c pffft.h ${SIMD_FLOAT_HDRS} )
  if (INSTALL_PFFFT)
    set(INSTALL_HEADERS ${INSTALL_HEADERS} pffft.h)
  endif()
else()
  set( FLOAT_SOURCES  )
endif()


if (USE_TYPE_DOUBLE)
  set( DOUBLE_SOURCES pffft_double.c pffft_double.h ${SIMD_DOUBLE_HDRS} )
  if (INSTALL_PFFFT)
    set(INSTALL_HEADERS ${INSTALL_HEADERS} pffft_double.h)
  endif()
else()
  set( DOUBLE_SOURCES )
endif()

######################################################

add_library(PFFFT STATIC ${FLOAT_SOURCES} ${DOUBLE_SOURCES} pffft_common.c pffft_priv_impl.h pffft.hpp )
set_target_properties(PFFFT PROPERTIES OUTPUT_NAME "pffft")
target_compile_definitions(PFFFT PRIVATE _USE_MATH_DEFINES)
if (USE_SCALAR_VECT)
  target_compile_definitions(PFFFT PRIVATE PFFFT_SCALVEC_ENABLED=1)
endif()
if (USE_DEBUG_ASAN)
  target_compile_options(PFFFT PRIVATE "-fsanitize=address")
endif()
if (NOT USE_SIMD)
  target_compile_definitions(PFFFT PRIVATE PFFFT_SIMD_DISABLE=1)
endif()
if (USE_SIMD AND USE_SIMD_NEON)
  target_compile_definitions(PFFFT PRIVATE PFFFT_ENABLE_NEON=1)
  target_compile_options(PFFFT PRIVATE "-mfpu=neon")
endif()
if (USE_SIMD AND USE_TYPE_DOUBLE)
  if(WIN32)
    if(DISABLE_SIMD_AVX)
      set_property(SOURCE pffft_double.c PROPERTY COMPILE_FLAGS "/arch:SSE2")
    else()
      set_property(SOURCE pffft_double.c PROPERTY COMPILE_FLAGS "/arch:AVX")
    endif()
  else()
    set_property(SOURCE pffft_double.c PROPERTY COMPILE_FLAGS "-march=native")
  endif()
  if(DISABLE_SIMD_AVX)
    target_compile_definitions(PFFFT PRIVATE PFFFT_AVX_DISABLE=1)
  endif()
endif()
target_link_libraries( PFFFT ${MATHLIB} )
set_property(TARGET PFFFT APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
if (INSTALL_PFFFT)
  set(INSTALL_TARGETS ${INSTALL_TARGETS} PFFFT)
  set(INSTALL_HEADERS ${INSTALL_HEADERS} pffft.hpp)
endif()

######################################################

if (USE_TYPE_FLOAT)

  add_library(PFDSP STATIC pf_mixer.cpp pf_mixer.h pf_carrier.cpp pf_carrier.h pf_cic.cpp pf_cic.h fmv.h )
  set_target_properties(PFDSP PROPERTIES OUTPUT_NAME "pfdsp")
  target_compile_definitions(PFDSP PRIVATE _USE_MATH_DEFINES)
  if (USE_DEBUG_ASAN)
    target_compile_options(PFDSP PRIVATE "-fsanitize=address")
  endif()
  if (USE_SIMD AND USE_SIMD_NEON)
    target_compile_definitions(PFDSP PRIVATE PFFFT_ENABLE_NEON=1)
    target_compile_options(PFDSP PRIVATE "-march=armv7-a" "-mfpu=neon")
  endif()
  target_link_libraries( PFDSP ${MATHLIB} )
  set_property(TARGET PFDSP APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  )
  if (INSTALL_PFDSP)
    set(INSTALL_TARGETS ${INSTALL_TARGETS} PFDSP)
    set(INSTALL_HEADERS ${INSTALL_HEADERS} pf_mixer.h pf_carrier.h pf_cic.h)
  endif()
endif()

######################################################

if (USE_FFTPACK)

  # float / single precision
  add_library(FFTPACK_FLOAT STATIC fftpack.c fftpack.h)
  target_compile_definitions(FFTPACK_FLOAT PRIVATE _USE_MATH_DEFINES)
  target_compile_options(FFTPACK_FLOAT PRIVATE $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>)
  target_link_libraries( FFTPACK_FLOAT ${MATHLIB} )
  set_property(TARGET FFTPACK_FLOAT APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  )

  # double precision
  add_library(FFTPACK_DOUBLE STATIC fftpack.c fftpack.h)
  target_compile_definitions(FFTPACK_DOUBLE PRIVATE _USE_MATH_DEFINES)
  target_compile_definitions(FFTPACK_DOUBLE PUBLIC FFTPACK_DOUBLE_PRECISION)
  target_compile_options(FFTPACK_DOUBLE PRIVATE $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>)
  target_link_libraries( FFTPACK_DOUBLE ${MATHLIB} )
  set_property(TARGET FFTPACK_DOUBLE APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  )

endif()

######################################################

if (USE_TYPE_FLOAT)
  # only 'float' supported in PFFASTCONV
  add_library(PFFASTCONV STATIC pffastconv.c pffastconv.h pffft.h )
  set_target_properties(PFFASTCONV PROPERTIES OUTPUT_NAME "pffastconv")
  target_compile_definitions(PFFASTCONV PRIVATE _USE_MATH_DEFINES)
  if (USE_DEBUG_ASAN)
    target_compile_options(PFFASTCONV PRIVATE "-fsanitize=address")
  endif()
  target_link_libraries( PFFASTCONV PFFFT ${ASANLIB} ${MATHLIB} )
  set_property(TARGET PFFASTCONV APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  )
  if (INSTALL_PFFASTCONV)
    set(INSTALL_TARGETS ${INSTALL_TARGETS} PFFASTCONV)
    set(INSTALL_HEADERS ${INSTALL_HEADERS} pffastconv.h)
  endif()
endif()


######################################################

install( TARGETS ${INSTALL_TARGETS}  DESTINATION lib)
install( FILES  ${INSTALL_HEADERS}  DESTINATION include)
