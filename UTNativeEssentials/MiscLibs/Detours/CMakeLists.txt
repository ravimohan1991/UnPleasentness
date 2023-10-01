#[[
    Abstractions and Models are NOT at WAR!
                                            - Cobwoy's Talisman
    But Abstractions don't care and Models can't understand!!
                                            - Lul, Practicality
 ]]
 
cmake_minimum_required (VERSION 3.23)

add_compile_definitions(_X86_)

project(Detours)

file(GLOB DET_HEADERS src/*.h)
#file(GLOB DET_SOURCE src/*.cpp)

add_library(Detours STATIC ${DET_HEADERS}
    src/creatwth.cpp
    src/detours.cpp
    src/disasm.cpp
    src/disolarm.cpp
    src/disolx86.cpp
    src/image.cpp
    src/modules.cpp
            )

# INTERFACE (Something that Consumers require but Producers don't)
# Since I am a library coder here, I am the Producer
# Rest of the Engine is Consumer here

target_include_directories(Detours
    PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(Detours PUBLIC Detours)
