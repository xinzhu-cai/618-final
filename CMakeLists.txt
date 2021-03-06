cmake_minimum_required(VERSION 3.15)
project(618_final)

set(CMAKE_CXX_STANDARD 17)

# move built executables to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# import googletest
add_compile_options(-Wno-deprecated-copy)
add_subdirectory(third_party/googletest)
include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${gmock_SOURCE_DIR}/include)

# import google benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark tests" FORCE)
add_subdirectory(third_party/benchmark)
include_directories(third_party/benchmark/include)

# concurrent queue headers
include_directories(include)

# TSX support
add_compile_options(-mrtm)

# double-word CAS support
# Note: libatomic detects CPU at runtime (dynamic link time), and will use the
#       cmpxchg16b ver if supported.
# Ref: https://stackoverflow.com/questions/49848793/genuinely-test-stdatomic-is-lock-free-or-not/49848879
add_link_options(-latomic)

# unittest executable
add_executable(unittest src/test.cc)
target_link_libraries(unittest gtest gtest_main)

# optimizations
add_compile_definitions(ENABLE_CACHELINE_ALIGNMENT)

# benchmark executable
add_executable(run_benchmark src/main.cc)
target_link_libraries(run_benchmark benchmark benchmark_main)
