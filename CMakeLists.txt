cmake_minimum_required(VERSION 3.14)

project(adhan VERSION 1.0.1 LANGUAGES C CXX)

# Set C standard to C17 (modern standard)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Enable more comprehensive warnings
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wstrict-prototypes -Wmissing-prototypes")
endif()

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

add_library(adhan STATIC
    src/astronomical.c
    src/solar_coordinates.c
    src/solar_time.c
    src/calculation_parameters.c
    src/prayer_times.c
    src/calendrical_helper.c
)

# Set target-specific properties
target_compile_features(adhan PUBLIC c_std_17)

# Add compile options for better code quality
target_compile_options(adhan PRIVATE
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Wstrict-prototypes -Wmissing-prototypes>
    $<$<C_COMPILER_ID:MSVC>:/W4>
)

target_include_directories(adhan PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Link math library on Unix-like systems
target_link_libraries(adhan PUBLIC $<$<PLATFORM_ID:Linux,Darwin>:m>)

# Build example binary
add_executable(example src/example.c)
target_link_libraries(example PRIVATE adhan)

include(CTest)
enable_testing()

# Use FetchContent for GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

set(test_SRCS
    test/Test.cpp
    test/astronomical_test.cpp
    test/double_utils_test.cpp
    test/calculation_method_test.cpp
    test/calculation_parameters_test.cpp
    test/prayer_times_test.cpp
)

add_executable(runUnitTests ${test_SRCS})
add_dependencies(runUnitTests adhan)

target_compile_options(runUnitTests PRIVATE -fpermissive)

target_link_libraries(runUnitTests
    PRIVATE
        gtest
        gmock
        adhan
        m
)

include(GoogleTest)
gtest_discover_tests(runUnitTests)
