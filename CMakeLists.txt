cmake_minimum_required(VERSION 3.21...3.31)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SetupToolchain)
include(ConfigureTarget)
include(CompilerWarnings)

project(template LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setup a 'solution' target
file(GLOB SOLUTION_SRC CONFIGURE_DEPENDS src/*.cpp src/*.h)
add_library(solution ${SOLUTION_SRC})
target_include_directories(solution PUBLIC src)
set_target_properties(solution PROPERTIES LINKER_LANGUAGE CXX)
ct_configure_target(solution)

# Setup a 'tests' target
file(GLOB TESTS_SRC CONFIGURE_DEPENDS test/*.cpp test/*.h)
add_executable(tests ${TESTS_SRC})
target_include_directories(tests PRIVATE test)
ct_configure_target(tests)

# Link tests with solution
target_link_libraries(tests PRIVATE solution)

# Link tests with dependencies
find_package(Catch2 REQUIRED)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# Enable warnings
option(CT_TREAT_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
ct_set_compiler_warnings(solution ${CT_TREAT_WARNINGS_AS_ERRORS})
ct_set_compiler_warnings(tests ${CT_TREAT_WARNINGS_AS_ERRORS})
