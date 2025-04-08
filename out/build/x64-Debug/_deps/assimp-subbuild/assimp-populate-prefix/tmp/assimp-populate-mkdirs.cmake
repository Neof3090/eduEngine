# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-src"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-build"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-subbuild/assimp-populate-prefix"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-subbuild/assimp-populate-prefix/tmp"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-subbuild/assimp-populate-prefix/src/assimp-populate-stamp"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-subbuild/assimp-populate-prefix/src"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-subbuild/assimp-populate-prefix/src/assimp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-subbuild/assimp-populate-prefix/src/assimp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/assimp-subbuild/assimp-populate-prefix/src/assimp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
