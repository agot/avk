set(BOOST_ENABLE_CMAKE ON)
if(WIN32)
  set(BOOST_CONFIGURE_COMMAND ./bootstrap.bat)
  set(BOOST_BUILD_COMMAND ./b2 install --prefix=${CMAKE_CURRENT_BINARY_DIR}/external toolset=msvc threading=multi link=static variant=debug,release address-model=64)
elseif(UNIX)
  set(BOOST_CONFIGURE_COMMAND ./bootstrap.sh)
  set(BOOST_BUILD_COMMAND ./b2 install --prefix=${CMAKE_CURRENT_BINARY_DIR}/external toolset=gcc threading=multi link=static variant=release address-model=64)
endif()
ExternalProject_Add(libboost
  URL https://boostorg.jfrog.io/artifactory/main/release/1.76.0/source/boost_1_76_0.tar.gz
  BUILD_IN_SOURCE true
  CONFIGURE_COMMAND ${BOOST_CONFIGURE_COMMAND}
  BUILD_COMMAND ${BOOST_BUILD_COMMAND}
  INSTALL_COMMAND ""
  PATCH_COMMAND ""
  UPDATE_COMMAND ""
)
