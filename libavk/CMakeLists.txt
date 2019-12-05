cmake_minimum_required(VERSION 3.20)

project(avk)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find thread
find_package(Threads REQUIRED)

# find vulkan
find_package(Vulkan REQUIRED)

if(UNIX AND NOT APPLE)
  # ECM
  find_package(ECM REQUIRED NO_MODULE)
  set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
  # find wayland
  find_package(Wayland REQUIRED)
endif()

# include external libraries
include(ExternalProject)
## boost
include("external/boost.cmake")
## libraries
include("external/libraries.cmake")

file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(avk STATIC)
add_dependencies(avk libboost libgsl libeigen libz libpng libglfw3)
target_sources(
  avk
  PRIVATE
  ${HEADERS}
  ${SOURCES}
)
target_compile_features(
  avk
  PRIVATE
  cxx_std_20
)
target_compile_definitions(
  avk
  PRIVATE
  $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

target_compile_options(
  avk
  PUBLIC
  $<$<PLATFORM_ID:Linux>:-fno-rtti -Wno-deprecated-enum-enum-conversion>
  $<$<PLATFORM_ID:Windows>:/GR>
)
target_include_directories(
  avk
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/external/include
  ${Vulkan_INCLUDE_DIRS}
)
target_link_directories(
  avk
  PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}/external/lib
)
target_link_libraries(
  avk
  PUBLIC
  ${CMAKE_THREAD_LIBS_INIT}
  ${Vulkan_LIBRARIES}
  $<$<PLATFORM_ID:Linux>:dl ${Wayland_LIBRARIES}>
  $<$<PLATFORM_ID:Linux>:libz.a libpng16.a libglfw3.a libboost_program_options.a>
  $<$<PLATFORM_ID:Windows>:zlibstatic libpng16_static glfw3>
)
set_target_properties(
  avk
  PROPERTIES
  CXX_CLANG_TIDY "clang-tidy;--checks=cppcoreguidelines-*,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,readability-*;--system-headers"
)
