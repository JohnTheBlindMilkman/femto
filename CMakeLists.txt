cmake_minimum_required(VERSION 3.1...3.28)
project(nInteg LANGUAGES CXX)

#find_package(Boost REQUIRED)

# set the directory for binaries and libs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# add some extra compile flags
add_compile_options(-Wall -Wextra -Wpedantic -Werror -fconcepts-diagnostics-depth=3)

add_executable(test main.cxx)
target_compile_features(test PUBLIC cxx_std_20)

add_executable(test2 main2.cxx)
target_compile_features(test2 PUBLIC cxx_std_20)

add_executable(test3 main3.cxx)
target_compile_features(test3 PUBLIC cxx_std_20)