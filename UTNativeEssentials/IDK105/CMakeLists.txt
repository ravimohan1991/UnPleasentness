#[[
    Abstractions and Models are NOT at WAR!
                                            - Cobwoy's Talisman
    But Abstractions don't care and Models can't understand!!
                                            - Lul, Practicality
 ]]

cmake_minimum_required(VERSION 3.0.0)

project(IDK)

file(GLOB IDK_HEADERS Src/*.h)

add_library(IDK SHARED Src/main.cpp ${IDK_HEADERS})

# INTERFACE (Something that Consumers require but Producers don't)
# Since I am a library coder here, I am the Producer
# Rest of the Engine is Consumer here

target_include_directories(IDK
    PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/Src
	if(LINUX)
		${CMAKE_CURRENT_SOURCE_DIR}/../MiscLibs/LinuxDetours/LinuxDetours
	elseif(WINDOWS)
		${CMAKE_CURRENT_SOURCE_DIR}/../MiscLibs/Detours/src
	endif()
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Src>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(IDK PUBLIC IDK)
if(MSVC)
    target_link_options(IDK PUBLIC /SAFESEH:NO)
endif()
