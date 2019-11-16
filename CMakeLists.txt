cmake_minimum_required(VERSION 3.15)
project(MinimoteServer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -lX11 -lXtst")

add_executable(MinimoteServer main.c minimote_server.h minimote_server.c minimote_utils.c minimote_utils.h minimote_defs.h minimote_packet_type.h minimote_packet_type.c minimote_packet.h minimote_packet.c minimote_x11.c minimote_x11.h minimote_controller.c minimote_controller.h)