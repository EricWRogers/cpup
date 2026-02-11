cmake_minimum_required(VERSION 3.10)
project(fogpi C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/>)

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
  message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
  set(CMAKE_C_COMPILER_LAUNCHER  "${CCACHE_PROGRAM}")
else()
    message(STATUS "ccache not found")
endif()

if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  find_program(MOLD_LINKER mold)
  find_program(LLD_LINKER  ld.lld)

  if (MOLD_LINKER)
    message(STATUS "Using mold linker: ${MOLD_LINKER}")
    add_link_options(-fuse-ld=mold)
  elseif (LLD_LINKER)
    message(STATUS "Using lld linker: ${LLD_LINKER}")
    add_link_options(-fuse-ld=lld)
  else()
    message(STATUS "No mold/lld found; using default linker")
  endif()
endif()

set_property(GLOBAL PROPERTY CMAKE_CONFIGURE_DEPENDS ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(SDL_OPENGLES ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)

set(glew-cmake_BUILD_SHARED         FALSE)
set(glew-cmake_BUILD_STATIC         TRUE)

if( NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
add_subdirectory(external/GL)
endif()

add_subdirectory(external/SDL)

file(GLOB_RECURSE SRC_SOURCES src/*.c)
file(GLOB_RECURSE SRC_HEADERS src/*.h)

add_executable(${PROJECT_NAME} ${SRC_SOURCES} ${SRC_HEADERS})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    PRIVATE
    SDL3::SDL3
    libglew_static
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    PRIVATE
    SDL3::SDL3
    libglew_static
)

target_precompile_headers(${PROJECT_NAME}
    PRIVATE
    <stdio.h>
    <stdlib.h>
    <string.h>

    "src/cpup/vec.h"
    "src/cpup/arena.h"
    "src/cpup/io.h"
    "src/cpup/types.h"
    "src/cpup/opengl.h"
)