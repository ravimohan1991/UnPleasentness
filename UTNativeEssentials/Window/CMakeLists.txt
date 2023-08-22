#[[
    Abstractions and Models are NOT at WAR!
                                            - Cobwoy's Talisman
    But Abstractions don't care and Models can't understand!!
                                            - Lul, Practicality
 ]]

cmake_minimum_required(VERSION 3.0.0)

project(Window)

file(GLOB WINDOW_HEADERS Inc/*.h)

add_library(Window SHARED IMPORTED)

# INTERFACE (Something that Consumers require but Producers don't)
# Since I am a library coder here, I am the Producer
# Rest of the Engine is Consumer here

target_include_directories(Window
    INTERFACE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)