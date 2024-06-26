cmake_minimum_required(VERSION 3.13)
include(FetchContent)
include(CheckCXXCompilerFlag)
set(FETCHCONTENT_QUIET OFF)

# FetchContent_MakeAvailable was introduced in 3.14
# there also isn't a version that accepts extra args at the end for add_subdirectory
# hence we write one ourselves
macro(FetchContent_MakeAvailable_Args NAME ARGS)
    FetchContent_GetProperties(${NAME})
    if(NOT ${NAME}_POPULATED)
        FetchContent_Populate(${NAME})
        add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR} ${ARGS})
    endif()
endmacro()

# Need an additional macro for LLVM as a sub-directory needs to be targeted for llvm-14.0.5
macro(FetchContent_MakeAvailable_SubDir_Args NAME SUBDIR ARGS)
    FetchContent_GetProperties(${NAME})
    if(NOT ${NAME}_POPULATED)
        FetchContent_Populate(${NAME})
        add_subdirectory(${${NAME}_SOURCE_DIR}/${SUBDIR}/ ${${NAME}_BINARY_DIR} ${ARGS})
    endif()
endmacro()

# we don't use git for LLVM here as it clones the entire LLVM repo which takes too long and we only need a small part of it

FetchContent_Declare(
  fmt
  GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
  GIT_PROGRESS    TRUE
)

FetchContent_Declare(
  llvm
  URL      https://github.com/llvm/llvm-project/releases/download/llvmorg-14.0.5/llvm-14.0.5.src.tar.xz
  URL_HASH MD5=6bd202e403d950c78985048ce499a518
)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  # this versions requires CMake >= 2.8.12 which silences the deprecation warning
  GIT_TAG        release-1.11.0
  GIT_PROGRESS   TRUE
)

FetchContent_Declare(
  capstone-lib
  GIT_REPOSITORY https://github.com/UoB-HPC/capstone.git
  GIT_TAG next


  # Old Git tag pre-Armv9.2
  # GIT_TAG e7be7d99e718ef9741026b80fc6f5e100fdf4f94 # trunk
)

cmake_policy(SET CMP0048 NEW)
project(SimEng VERSION 0.9.6 LANGUAGES C CXX)

# If no build type was defined, default to Release
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to Release as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE
      STRING "Choose the type of build." FORCE)
endif()

# Require and enable C++17 support, and disable compiler extensions.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configure RPATH
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_MACOSX_RPATH 1)

# Enable PIC for libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable additional compiler warnings for all targets
add_compile_options(-Wall)

# Disable RTTI for all targets
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)

# Include SimEng API headers in all targets and install them
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
install(DIRECTORY src/include/simeng
        DESTINATION include
        FILES_MATCHING PATTERN "*.hh")

# capstone
set(CAPSTONE_BUILD_TESTS OFF CACHE BOOL "Disable Capstone tests")
set(CAPSTONE_BUILD_SHARED OFF CACHE BOOL "Disable Capstone shared library")
set(CAPSTONE_BUILD_CSTOOL OFF CACHE BOOL "Disable cstool build")
set(CAPSTONE_INSTALL OFF CACHE BOOL "Disable install of capstone")

set(CAPSTONE_ARM_SUPPORT OFF CACHE BOOL "Disable A32 support")
set(CAPSTONE_MIPS_SUPPORT OFF CACHE BOOL "Disable MIPS support")
set(CAPSTONE_X86_SUPPORT OFF CACHE BOOL "Disable x86 support")
set(CAPSTONE_PPC_SUPPORT OFF CACHE BOOL "Disable PowerPC support")
set(CAPSTONE_SPARC_SUPPORT OFF CACHE BOOL "Disable Sparc support")
set(CAPSTONE_SYSZ_SUPPORT OFF CACHE BOOL "Disable SystemZ support")
set(CAPSTONE_XCORE_SUPPORT OFF CACHE BOOL "Disable XCore support")
set(CAPSTONE_M68K_SUPPORT OFF CACHE BOOL "Disable M68K support")
set(CAPSTONE_TMS320C64X_SUPPORT OFF CACHE BOOL "Disable TMS320C64x")
set(CAPSTONE_M680X_SUPPORT OFF CACHE BOOL "Disable M680x support")
set(CAPSTONE_EVM_SUPPORT OFF CACHE BOOL "Disable EVM support")
set(CAPSTONE_MOS65XX_SUPPORT OFF CACHE BOOL "Disable MSO65XX support")
set(CAPSTONE_WASM_SUPPORT OFF CACHE BOOL "Disable WASM support")
set(CAPSTONE_BPF_SUPPORT OFF CACHE BOOL "Disable BPF support")

FetchContent_MakeAvailable_Args(capstone-lib EXCLUDE_FROM_ALL)
include_directories("${capstone_BINARY_DIR}/include" "${capstone_SOURCE_DIR}/include")

# Enable the single RYML header to be packaged into the SimEng shared library
set(RYML_SHARED ON)

option(SIMENG_ENABLE_TESTS "Whether to enable testing for SimEng" OFF)
option(SIMENG_USE_EXTERNAL_LLVM "Use an external LLVM rather than building it as a submodule" OFF)
option(SIMENG_SANITIZE "Enable compiler sanitizers" OFF)
option(SIMENG_OPTIMIZE "Enable Extra Compiler Optimizations" OFF)
option(SIMENG_ENABLE_SST "Compile SimEng SST Wrapper" OFF)
option(SIMENG_ENABLE_SST_TESTS "Enable testing for SST" OFF)

FetchContent_MakeAvailable_Args(fmt EXCLUDE_FROM_ALL)
include_directories("${fmt_BINARY_DIR}/include" "${fmt_SOURCE_DIR}/include")
# Set CXX flag for Apple Mac so that `binary_function` and `unary_function` types that are used in SST can be recognised. 
# They were deprecated in C++11 and removed in C++17, and Apple Clang v15 no longer supports these types without the following flag
# TODO: Remove once SST integration has updated to SST version 13 or later - the use of unary and binary functions are removed in later versions.
if(APPLE)
  add_compile_definitions(_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION)
endif()

if (SIMENG_OPTIMIZE)
  # Turn on link time optimization for all targets.
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

  check_cxx_compiler_flag(-march=native X86)

  if(X86)
    add_compile_options(-march=native -mtune=native)
    add_link_options(-march=native -mtune=native)
  endif()
endif()

set(SANITIZE_OPTIONS -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fno-sanitize-recover=address,undefined)
if (SIMENG_SANITIZE)
    add_compile_options(${SANITIZE_OPTIONS})
    add_link_options(${SANITIZE_OPTIONS})
endif()

if(SIMENG_ENABLE_TESTS)

  ## Setup LLVM ##
  if (SIMENG_USE_EXTERNAL_LLVM)
    find_package(LLVM REQUIRED CONFIG NO_CMAKE_BUILDS_PATH)

    #   # Check LLVM version
    # if ((${LLVM_PACKAGE_VERSION} VERSION_LESS "8.0") OR (${LLVM_PACKAGE_VERSION} VERSION_GREATER_EQUAL "14.1"))
    #   message(FATAL_ERROR "LLVM version must be >= 8.0 and <= 14.0")
    # endif()

    #   # Print message containing if the full test suite will run
    # if (${LLVM_PACKAGE_VERSION} VERSION_LESS "14.0")
    #   message(STATUS "LLVM version does not support AArch64 extensions SME or SVE2. These test suites will be skipped.")
    # endif()

  else()

    set(LLVM_TARGETS_TO_BUILD "AArch64;RISCV" CACHE INTERNAL "")

    set(LLVM_BUILD_RUNTIME OFF)

    set(LLVM_BUILD_TOOLS OFF)
    set(LLVM_INCLUDE_TOOLS OFF)

    set(LLVM_BUILD_EXAMPLES OFF)
    set(LLVM_INCLUDE_EXAMPLES OFF)

    set(LLVM_BUILD_TESTS OFF)
    set(LLVM_INCLUDE_TESTS OFF)

    set(LLVM_BUILD_BENCHMARKS OFF)
    set(LLVM_INCLUDE_BENCHMARKS OFF)

    set(LLVM_BUILD_DOCS OFF)
    set(LLVM_INCLUDE_DOCS OFF)

    set(LLVM_INCLUDE_DOCS OFF)
    set(LLVM_ENABLE_BINDINGS OFF)
    set(LLVM_INSTALL_UTILS OFF)

    # XXX all LLVM specific cmake variables must be set BEFORE FetchContent_MakeAvailable otherwise they have no effect
    FetchContent_MakeAvailable_SubDir_Args(llvm llvm-14.0.5.src EXCLUDE_FROM_ALL)
    # make sure we get the headers too
    include_directories("${llvm_BINARY_DIR}/include" "${llvm_SOURCE_DIR}/include")

    find_package(LLVM REQUIRED CONFIG NO_DEFAULT_PATH
                 PATHS "${llvm_BINARY_DIR}/lib/cmake/llvm")

    # NOTE: we don't do the usual version checks here because it needs vars exported in find_LLVM
    # we just assume it's good because it must be whitelisted in FetchContent_Declare
endif()

  set(SIMENG_LLVM_VERSION ${LLVM_VERSION_MAJOR} CACHE INTERNAL "LLVM major version number used.")
  message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
  message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

  # Check LLVM was built with the correct targets enabled
  if (NOT "AArch64" IN_LIST LLVM_TARGETS_TO_BUILD)
    message(FATAL_ERROR "LLVM was built without AArch64 target")
  endif()
  if (NOT "RISCV" IN_LIST LLVM_TARGETS_TO_BUILD)
      message(FATAL_ERROR "LLVM was built without RISC-V target")
  endif()

  ## Setup googletest ##
  FetchContent_MakeAvailable_Args(googletest EXCLUDE_FROM_ALL)
  enable_testing()

  add_subdirectory(test)
  # saves us from having to build all targets before running the tests
  add_custom_target(test-all
    COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS unittests regression-aarch64 regression-riscv integrationtests
  )
endif()

# include sources
add_subdirectory(src)
add_subdirectory(docs)

if (SIMENG_ENABLE_SST)
  if (SST_INSTALL_DIR)
    add_subdirectory(sst)
    if (SIMENG_ENABLE_SST_TESTS)
      enable_testing()
      list(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")
      add_custom_target(test-sst-simeng
      COMMAND ${CMAKE_CTEST_COMMAND}
      DEPENDS sstsimengtest
      )
    endif()
  else()
    message(WARNING "SST build was selected but SST install directory was not specified.
    Please specify -DSST_INSTALL_DIR=<path> for the SST build to proceed.")  
  endif()
endif()

# Install SimEng model configs in the build directory
set(SIMENG_CONFIG_INSTALL_DIR "${CMAKE_BINARY_DIR}/simeng-configs")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/configs/ DESTINATION ${SIMENG_CONFIG_INSTALL_DIR})