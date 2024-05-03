cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(Chess)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

set(RAYGUI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(RAYGUI_SRC ${RAYGUI_ROOT}/src)
set(RAYGUI_EXAMPLES ${RAYGUI_ROOT}/examples)

add_library(raygui INTERFACE)
file(GLOB sources ${RAYGUI_ROOT}/src/*.h)
set(RAYGUI_HEADERS ${sources})
install(FILES
    ${RAYGUI_HEADERS} DESTINATION include
)
target_include_directories(raygui INTERFACE ${RAYGUI_SRC})

# Our Project

add_executable(${PROJECT_NAME} src/main.cpp)
#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)
file(COPY images audio DESTINATION ${CMAKE_BINARY_DIR})

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
