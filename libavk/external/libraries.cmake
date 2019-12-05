ExternalProject_Add(libgsl
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG v3.1.0
  GIT_SHALLOW ON
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external -DCMAKE_BUILD_TYPE=Release
  PATCH_COMMAND ""
  INSTALL_COMMAND cmake --build . --target install --config Release
  UPDATE_COMMAND ""
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libgsl-build
)
ExternalProject_Add(libz
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG v1.2.11
  GIT_SHALLOW ON
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external -DCMAKE_BUILD_TYPE=Release
  PATCH_COMMAND ""
  INSTALL_COMMAND cmake --build . --target install --config Release
  UPDATE_COMMAND ""
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libz-build
)
ExternalProject_Add(libpng
  GIT_REPOSITORY git://git.code.sf.net/p/libpng/code
  GIT_TAG v1.6.37
  GIT_SHALLOW ON
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external -DCMAKE_BUILD_TYPE=Release
  PATCH_COMMAND ""
  INSTALL_COMMAND cmake --build . --target install --config Release
  UPDATE_COMMAND ""
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libpng-build
)
ExternalProject_Add(libeigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.3.9
  GIT_SHALLOW ON
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external -DINCLUDE_INSTALL_DIR=include -DCMAKE_BUILD_TYPE=Release
  PATCH_COMMAND ""
  INSTALL_COMMAND cmake --build . --target install --config Release
  UPDATE_COMMAND ""
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libeigen-build
)
ExternalProject_Add(libglfw3
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.3
  GIT_SHALLOW ON
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external -DCMAKE_BUILD_TYPE=Release -DGLFW_USE_WAYLAND=ON -DGLFW_VULKAN_STATIC=ON -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF
  PATCH_COMMAND ""
  INSTALL_COMMAND cmake --build . --target install --config Release
  UPDATE_COMMAND ""
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libglfw3-build
)
add_dependencies(libpng libz)
