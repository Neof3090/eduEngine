#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

set(ASSIMP_BUILD_SHARED_LIBS ON)

if(MSVC)
  if(MSVC_TOOLSET_VERSION)
    set(MSVC_PREFIX "vc${MSVC_TOOLSET_VERSION}")
  else()
    if( MSVC70 OR MSVC71 )
      set(MSVC_PREFIX "vc70")
    elseif( MSVC80 )
      set(MSVC_PREFIX "vc80")
    elseif( MSVC90 )
      set(MSVC_PREFIX "vc90")
    elseif( MSVC10 )
      set(MSVC_PREFIX "vc100")
    elseif( MSVC11 )
      set(MSVC_PREFIX "vc110")
    elseif( MSVC12 )
      set(MSVC_PREFIX "vc120")
    elseif( MSVC_VERSION LESS 1910)
      set(MSVC_PREFIX "vc140")
    elseif( MSVC_VERSION LESS 1920)
      set(MSVC_PREFIX "vc141")
    elseif( MSVC_VERSION LESS 1930)
      set(MSVC_PREFIX "vc142")
    else()
      set(MSVC_PREFIX "vc150")
    endif()
  endif()
  set(ASSIMP_LIBRARY_SUFFIX "-${MSVC_PREFIX}-mt" CACHE STRING "the suffix for the assimp windows library" )

  if(ASSIMP_BUILD_SHARED_LIBS)
    set(sharedLibraryName "assimp${ASSIMP_LIBRARY_SUFFIX}d.dll")
    set(importLibraryName "assimp${ASSIMP_LIBRARY_SUFFIX}d.lib")

    # Import target "assimp::assimp" for configuration "Debug"
    set_property(TARGET assimp::assimp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(assimp::assimp PROPERTIES
      IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/${importLibraryName}"
      IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/${sharedLibraryName}"
    )
    list(APPEND _IMPORT_CHECK_TARGETS assimp::assimp )
    list(APPEND _IMPORT_CHECK_FILES_FOR_assimp::assimp "${_IMPORT_PREFIX}/lib/${importLibraryName}")
    list(APPEND _IMPORT_CHECK_FILES_FOR_assimp::assimp "${_IMPORT_PREFIX}/bin/${sharedLibraryName}" )
  else()
    set(staticLibraryName "assimp${ASSIMP_LIBRARY_SUFFIX}d.lib")

    # Import target "assimp::assimp" for configuration "Debug"
    set_property(TARGET assimp::assimp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(assimp::assimp PROPERTIES
      IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/${staticLibraryName}"
    )
    list(APPEND _IMPORT_CHECK_TARGETS assimp::assimp )
    list(APPEND _IMPORT_CHECK_FILES_FOR_assimp::assimp "${_IMPORT_PREFIX}/lib/${staticLibraryName}")
  endif()

else()
  set(ASSIMP_LIBRARY_SUFFIX "" CACHE STRING "the suffix for the assimp libraries" )
  if(ASSIMP_BUILD_SHARED_LIBS)
    set(sharedLibraryName "libassimp${ASSIMP_LIBRARY_SUFFIX}d.dll.5")
    set_target_properties(assimp::assimp PROPERTIES
      IMPORTED_SONAME_DEBUG "${sharedLibraryName}"
      IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/${sharedLibraryName}"
    )
    list(APPEND _IMPORT_CHECK_TARGETS assimp::assimp )
    list(APPEND _IMPORT_CHECK_FILES_FOR_assimp::assimp "${_IMPORT_PREFIX}/lib/${sharedLibraryName}" )
  else()
    set(staticLibraryName "libassimp${ASSIMP_LIBRARY_SUFFIX}d.lib")
    set_target_properties(assimp::assimp PROPERTIES
      IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/${staticLibraryName}"
    )
    list(APPEND _IMPORT_CHECK_TARGETS assimp::assimp )
    list(APPEND _IMPORT_CHECK_FILES_FOR_assimp::assimp "${_IMPORT_PREFIX}/lib/${staticLibraryName}" )
  endif()
endif()




# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

get_filename_component(ASSIMP_ROOT_DIR "C:/Users/neofe/Source/Repos/Neof3090/eduEngine/out/install/x64-Debug" REALPATH)
set( ASSIMP_CXX_FLAGS ) # dynamically linked library
set( ASSIMP_LINK_FLAGS "" )
set( ASSIMP_LIBRARY_DIRS "${ASSIMP_ROOT_DIR}/lib")
set( ASSIMP_INCLUDE_DIRS "${ASSIMP_ROOT_DIR}/include")
if(ASSIMP_BUILD_SHARED_LIBS)
  set( ASSIMP_LIBRARIES ${sharedLibraryName})
else()
  set( ASSIMP_LIBRARIES ${staticLibraryName})
endif()

# for compatibility with pkg-config
set(ASSIMP_CFLAGS_OTHER "${ASSIMP_CXX_FLAGS}")
set(ASSIMP_LDFLAGS_OTHER "${ASSIMP_LINK_FLAGS}")

MARK_AS_ADVANCED(
  ASSIMP_ROOT_DIR
  ASSIMP_CXX_FLAGS
  ASSIMP_LINK_FLAGS
  ASSIMP_INCLUDE_DIRS
  ASSIMP_LIBRARIES
  ASSIMP_CFLAGS_OTHER
  ASSIMP_LDFLAGS_OTHER
  ASSIMP_LIBRARY_SUFFIX
  ASSIMP_BUILD_SHARED_LIBS
)
