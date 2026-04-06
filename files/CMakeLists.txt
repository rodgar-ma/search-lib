cmake_minimum_required(VERSION 3.25)

project(shortpath
    VERSION 0.1.0
    DESCRIPTION "Library of shortest-path algorithms for map routing using OpenStreetMap"
    LANGUAGES CXX
)

# ── Estándar ──────────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Opciones de build ─────────────────────────────────────────────────────────
option(SHORTPATH_BUILD_TESTS      "Build tests"       ON)
option(SHORTPATH_BUILD_BENCHMARKS "Build benchmarks"  ON)

# ── Flags de compilación por compilador ───────────────────────────────────────
add_library(shortpath_compile_options INTERFACE)

if(MSVC)
    target_compile_options(shortpath_compile_options INTERFACE
        /W4 /WX /permissive- /utf-8
    )
else()
    target_compile_options(shortpath_compile_options INTERFACE
        -Wall -Wextra -Wpedantic -Werror
        -Wno-unused-parameter
    )
endif()

# ── Dependencias (submódulos) ─────────────────────────────────────────────────
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Updating git submodules...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMODULE_RESULT
        )
        if(NOT GIT_SUBMODULE_RESULT EQUAL 0)
            message(FATAL_ERROR "git submodule update failed: ${GIT_SUBMODULE_RESULT}")
        endif()
    endif()
endif()

# libosmium (header-only + sus dependencias)
set(OSMIUM_INCLUDE_DIRS
    "${PROJECT_SOURCE_DIR}/third_party/libosmium/include"
    "${PROJECT_SOURCE_DIR}/third_party/protozero/include"
)
foreach(dir ${OSMIUM_INCLUDE_DIRS})
    if(NOT EXISTS "${dir}")
        message(FATAL_ERROR
            "Submódulo no encontrado: ${dir}\n"
            "Ejecuta: git submodule update --init --recursive"
        )
    endif()
endforeach()

find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(Threads REQUIRED)

# Catch2 para tests
if(SHORTPATH_BUILD_TESTS)
    set(CATCH2_DIR "${PROJECT_SOURCE_DIR}/third_party/Catch2")
    if(NOT EXISTS "${CATCH2_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "Submódulo Catch2 no encontrado en ${CATCH2_DIR}")
    endif()
    add_subdirectory(third_party/Catch2)
endif()

# Google Benchmark
if(SHORTPATH_BUILD_BENCHMARKS)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(GBENCH_DIR "${PROJECT_SOURCE_DIR}/third_party/benchmark")
    if(NOT EXISTS "${GBENCH_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "Submódulo benchmark no encontrado en ${GBENCH_DIR}")
    endif()
    add_subdirectory(third_party/benchmark)
endif()

# ── Librería principal ────────────────────────────────────────────────────────
add_library(shortpath
    src/graph/graph.cpp
    src/algorithms/dijkstra.cpp
    src/algorithms/astar.cpp
    src/parser/osm_parser.cpp
    src/utils/geo.cpp
)

target_include_directories(shortpath
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${OSMIUM_INCLUDE_DIRS}
)

target_link_libraries(shortpath
    PRIVATE
        shortpath_compile_options
        ZLIB::ZLIB
        BZip2::BZip2
        Threads::Threads
)

# ── Subdirectorios ────────────────────────────────────────────────────────────
if(SHORTPATH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(SHORTPATH_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
