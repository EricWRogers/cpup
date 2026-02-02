cmake_minimum_required(VERSION 3.10)
project(fogpi)

set(CMAKE_C_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/>)

file(GLOB_RECURSE SRC_SOURCES src/*.c)
file(GLOB_RECURSE SRC_HEADERS src/*.h)

add_executable(${PROJECT_NAME} ${SRC_SOURCES} ${SRC_HEADERS})

include(FetchContent)
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.2.0
)
set(SDL_PIPEWIRE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
