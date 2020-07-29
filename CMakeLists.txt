cmake_minimum_required(VERSION 3.15)
project(MinimoteServer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -lX11 -lXtst -lglib-2.0")

file(GLOB_RECURSE sources
        "src/*.c"
        "src/*.h"
)

include_directories(src)

add_executable(MinimoteServer ${sources})

target_link_libraries(MinimoteServer
    X11 Xtst glib-2.0
)