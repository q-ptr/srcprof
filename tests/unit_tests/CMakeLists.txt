
cmake_minimum_required(VERSION 3.20.0)


project(srcprof_unit_tests 
  HOMEPAGE_URL https://github.com/q-ptr/srcprof 
  DESCRIPTION "invasive cpu profiler - unit tests" 
  VERSION 0.0.1 
  LANGUAGES CXX)


add_executable(${PROJECT_NAME} 
  srcprof_unit_tests.cpp 
  "$ENV{CPP_PACKAGE_CATCHORG_CATCH2}/extras/catch_amalgamated.cpp" 
)


add_compile_definitions(CATCH_AMALGAMATED_CUSTOM_MAIN)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

add_compile_definitions(QPTR_SRCPROF_ENABLED)

target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{CPP_PACKAGE_CATCHORG_CATCH2}/extras")

target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{CPP_PACKAGE_QPTR_PREDEF}/include")

target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{CPP_PACKAGE_BOOSTORG_PREDEF}/include")

add_compile_definitions(FMT_HEADER_ONLY)
target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{CPP_PACKAGE_FMTLIB_FMT}/include")

target_include_directories(${PROJECT_NAME} PRIVATE "$ENV{CPP_PACKAGE_NEARGYE_SEMVER}/include")

target_include_directories(${PROJECT_NAME} INTERFACE "$ENV{CPP_PACKAGE_MARTINUS_UNORDERED_DENSE}/include")

target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../../include)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
