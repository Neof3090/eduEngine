# Install script for directory: C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/neofe/Source/Repos/Neof3090/eduEngine/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.0.0-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/assimp-5.0" TYPE FILE FILES
    "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-build/assimp-config.cmake"
    "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-build/assimp-config-version.cmake"
    "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-build/assimpTargets.cmake"
    "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-build/assimpTargets-debug.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.0.0-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-build/assimp.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-build/contrib/zlib/cmake_install.cmake")
  include("C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-build/contrib/cmake_install.cmake")
  include("C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-build/code/cmake_install.cmake")

endif()

