#[[
    Abstractions and Models are NOT at WAR!
                                            - Cobwoy's Talisman
    But Abstractions don't care and Models can't understand!!
                                            - Lul, Practicality
 ]]

# Ugly hack to make find_package/find_library find static libs first
if(APPLE)
  set(CMAKE_FIND_LIBRARY_SUFFIXES .a .dylib)
  set(CMAKE_FIND_FRAMEWORK LAST)
  set(SHARED_LIB_EXT .dylib)
elseif(UNIX)
  set(CMAKE_FIND_LIBRARY_SUFFIXES .a .so)
  set(SHARED_LIB_EXT .so)
else()
  set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll)
  set(SHARED_LIB_EXT .dll)
endif()
  
# SDL2/SDL2_ttf
if(UNIX)
  find_package(SDL2)
  if(NOT SDL2_FOUND)
    find_path(SDL2_INCLUDE_DIRS SDL.h
	  REQUIRED
	  HINTS
	  PATH_SUFFIXES include include/SDL2
    )
    find_library(SDL2_LIBRARY
      REQUIRED
   	  NAMES SDL2
   	  HINTS
   	  PATH_SUFFIXES lib
    )
  set(SDL2_LIBRARIES ${SDL2_LIBRARY})
  endif()
  message(STATUS "Found SDL2 headers: ${SDL2_INCLUDE_DIRS}")
  message(STATUS "Found SDL2 library: ${SDL2_LIBRARIES}")
  include_directories(${SDL2_INCLUDE_DIRS})
  find_path(SDL2_TTF_INCLUDE_DIR SDL_ttf.h
	REQUIRED
	HINTS
	PATH_SUFFIXES include include/SDL2
  )
  find_library(SDL2_TTF_LIBRARY
    REQUIRED
   	NAMES SDL2_ttf
   	HINTS
   	PATH_SUFFIXES lib
  )
  message(STATUS "Found SDL2_TTF headers: ${SDL2_TTF_INCLUDE_DIR}")
  message(STATUS "Found SDL2_TTF library: ${SDL2_TTF_LIBRARY}")  
  include_directories(${SDL2_TTF_INCLUDE_DIR})
  find_library(FREETYPE_LIBRARY
    REQUIRED
   	NAMES libfreetype freetype
   	HINTS
   	PATH_SUFFIXES lib
  )
  set(FREETYPE_LIBRARIES "${FREETYPE_LIBRARY}")
  message(STATUS "Found FreeType library: ${FREETYPE_LIBRARIES}")
endif()

# zlib
#[[
find_package(ZLIB REQUIRED)
message(STATUS "Found zlib headers: ${ZLIB_INCLUDE_DIR}")
message(STATUS "Found zlib library: ${ZLIB_LIBRARY}")
include_directories(${ZLIB_INCLUDE_DIR})]]

# Involved libraries and essentials
#add_subdirectory(zlib)


#[[
if(WINDOWS)
  set(FMOD_INSTALL_LIBRARY ${REPOSITORYROOT}/External/fmod/lib/${OLDUNREAL_CPU}/fmod.dll)
  set(FMOD_LINK_LIBRARY
    ${CMAKE_CURRENT_SOURCE_DIR}/External/fmod/lib/${OLDUNREAL_CPU}/fmod_vc.lib)
else()
  set(FMOD_INSTALL_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/External/fmod/lib/${OLDUNREAL_CPU}/libfmod${SHARED_LIB_EXT})
  set(FMOD_LINK_LIBRARY ${FMOD_INSTALL_LIBRARY})
endif()
]]

# Header-only or precompiled stuff
#[[
include_directories(${REPOSITORYROOT}/External/glm)
include_directories(${REPOSITORYROOT}/External/fmod/inc)
include_directories(${REPOSITORYROOT}/External/curl/include)
if(WINDOWS)
  include_directories(${REPOSITORYROOT}/External/dxsdk/Include)
elseif(APPLE)
  include_directories(External/metal-cpp)
endif()
]]

# Platform/distro packages and libs
if(LINUX)
  find_package(Threads REQUIRED)
  if (OLDUNREAL_BUILD_WX_LAUNCHER)
    set(wxWidgets_CONFIGURATION mswu)
    find_package(wxWidgets COMPONENTS core base REQUIRED)
    include(${wxWidgets_USE_FILE})
  endif()
elseif(WINDOWS)
  find_package(Threads REQUIRED)
elseif(APPLE)
  find_package(Threads REQUIRED)
  find_library(COCOA_FRAMEWORK Cocoa)
  find_library(METAL_FRAMEWORK Metal)
  find_library(FOUNDATION_FRAMEWORK Foundation)
  find_library(COREGRAPHICS_FRAMEWORK CoreGraphics)
  find_library(METALKIT_FRAMEWORK MetalKit)
endif()
