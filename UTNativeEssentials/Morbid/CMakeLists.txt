#[[
    Abstractions and Models are NOT at WAR!
                                            - Cobwoy's Talisman
    But Abstractions don't care and Models can't understand!!
                                            - Lul, Practicality
 ]]

cmake_minimum_required(VERSION 3.0.0)

project(Morbid)

file(GLOB Morbid_HEADERS HookMaster.h MorbiD-Windows.h)

add_library(Morbid SHARED MorbiD-Windows.cpp HookMaster.cpp ${Morbid_HEADERS})

# INTERFACE (Something that Consumers require but Producers don't)
# Since I am a library coder here, I am the Producer
# Rest of the Engine is Consumer here

target_include_directories(Morbid
    PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../MiscLibs/Detours/src
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(Morbid PUBLIC Morbid)

