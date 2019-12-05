cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "avkmain")
project(${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

## libavk
add_subdirectory(libavk)

## properties
add_executable(${PROJECT_NAME})
add_dependencies(${PROJECT_NAME} avk)
target_sources(
  ${PROJECT_NAME}
  PRIVATE
  main.cpp
)
target_compile_features(
  ${PROJECT_NAME}
  PRIVATE
  cxx_std_20
)
target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE
  $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)
target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
  -Wall
  $<$<PLATFORM_ID:Linux>:-fno-rtti>
  $<$<PLATFORM_ID:Linux>:$<$<CONFIG:Debug>:-O0 -g>>
  $<$<PLATFORM_ID:Linux>:$<$<CONFIG:Release>:-O2>>
  $<$<PLATFORM_ID:Linux>:$<$<CONFIG:RelWithDebInfo>:-O2 -g>>
)
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  avk
)
