# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/entt-src"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/entt-build"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/entt-subbuild/entt-populate-prefix"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/entt-subbuild/entt-populate-prefix/tmp"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/entt-subbuild/entt-populate-prefix/src/entt-populate-stamp"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/entt-subbuild/entt-populate-prefix/src"
  "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/entt-subbuild/entt-populate-prefix/src/entt-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/entt-subbuild/entt-populate-prefix/src/entt-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/neofe/source/repos/Neof3090/eduEngine/out/build/x64-Debug/_deps/entt-subbuild/entt-populate-prefix/src/entt-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
