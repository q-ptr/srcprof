
cmake_minimum_required(VERSION 3.20.0)


project(srcprof 
  HOMEPAGE_URL https://github.com/q-ptr/srcprof
  DESCRIPTION "invasive cpu profiler" 
  VERSION 0.0.1 
  LANGUAGES CXX)


add_library(${PROJECT_NAME} INTERFACE)


add_subdirectory(tests/tests)
add_subdirectory(tests/fuzz_tests)
add_subdirectory(tests/perf_tests)
add_subdirectory(tests/unit_tests)

add_compile_definitions(QPTR_SRCPROF_ENABLED)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

target_include_directories(${PROJECT_NAME} INTERFACE "$ENV{CPP_PACKAGE_QPTR_PREDEF}/include")

target_include_directories(${PROJECT_NAME} INTERFACE "$ENV{CPP_PACKAGE_BOOSTORG_PREDEF}/include")

add_compile_definitions(FMT_HEADER_ONLY)
target_include_directories(${PROJECT_NAME} INTERFACE "$ENV{CPP_PACKAGE_FMTLIB_FMT}/include")

target_include_directories(${PROJECT_NAME} INTERFACE "$ENV{CPP_PACKAGE_MARTINUS_UNORDERED_DENSE}/include")

target_include_directories(${PROJECT_NAME} INTERFACE "$ENV{CPP_PACKAGE_NEARGYE_SEMVER}/include")

target_include_directories(${PROJECT_NAME} INTERFACE ./include)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
