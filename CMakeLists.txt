cmake_minimum_required(VERSION 3.21)
project(minimote-server C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

file(GLOB_RECURSE sources "src/*.c" "src/*.h")
list(FILTER sources EXCLUDE REGEX ".*main.c$")

add_executable(minimote-server ${sources} "src/main.c")

target_include_directories(minimote-server PRIVATE src)

# wayland

find_package(WAYLAND_CLIENT)

if (WAYLAND_CLIENT_FOUND)
    message(STATUS "wayland-client has been found")
    message(STATUS "WAYLAND_CLIENT_INCLUDE_DIR=${WAYLAND_CLIENT_INCLUDE_DIR}")
    message(STATUS "WAYLAND_CLIENT_LIBRARY=${WAYLAND_CLIENT_LIBRARY}")
else()
    message(STATUS "wayland-client not found")
endif()

# x11

find_package(X11)

if(X11_FOUND)
    message(STATUS "X11 has been found")
    message(STATUS "X11_INCLUDE_DIR=${X11_INCLUDE_DIR}")
    message(STATUS "X11_LIBRARIES=${X11_LIBRARIES}")
else()
    message(STATUS "x11 not found")
endif()


# xkbcommon

find_package(XKBCOMMON REQUIRED)

if (XKBCOMMON_FOUND)
    message(STATUS "xkbcommon has been found")
    message(STATUS "XKBCOMMON_INCLUDE_DIRS=${XKBCOMMON_INCLUDE_DIRS}")
    message(STATUS "XKBCOMMON_LIBRARIES=${XKBCOMMON_LIBRARIES}")
else()
    message(FATAL_ERROR "xkbcommon not found")
endif()

# xkbcommon-x11

find_package(XKBCOMMON_X11)

if (XKBCOMMON_X11_FOUND)
    message(STATUS "xkbcommon has been found")
    message(STATUS "XKBCOMMON_X11_INCLUDE_DIRS=${XKBCOMMON_X11_INCLUDE_DIRS}")
    message(STATUS "XKBCOMMON_X11_LIBRARIES=${XKBCOMMON_X11_LIBRARIES}")
else()
    message(STATUS "xkbcommon-x11 not found")
endif()

# xcb

find_package(XCB)

if (XCB_FOUND)
    message(STATUS "xcb has been found")
    message(STATUS "XCB_INCLUDE_DIRS=${XCB_INCLUDE_DIRS}")
    message(STATUS "XCB_LIBRARIES=${XCB_LIBRARIES}")
else()
    message(STATUS "xcb not found")
endif()


target_link_libraries(minimote-server
    ${XKBCOMMON_LIBRARIES}
)

if (WAYLAND_CLIENT_FOUND)
    message(STATUS "Enabling Wayland support")
    add_compile_definitions(WAYLAND)
    include_directories(${WAYLAND_CLIENT_INCLUDE_DIRS})
    target_link_libraries(minimote-server
        ${WAYLAND_CLIENT_LIBRARIES}
    )
endif()
if(X11_FOUND AND XKBCOMMON_X11_FOUND AND XCB_FOUND)
    message(STATUS "Enabling X11 support")
    add_compile_definitions(X11)
    include_directories(${XKBCOMMON_INCLUDE_DIRS})
    include_directories(${XKBCOMMON_INCLUDE_DIRS})
    include_directories(${XCB_INCLUDE_DIRS})
    target_link_libraries(minimote-server
        ${XKBCOMMON_LIBRARIES} ${XKBCOMMON_X11_LIBRARIES} ${XCB_LIBRARIES}
    )
endif()

if (NOT WAYLAND_CLIENT_FOUND AND NOT (X11_FOUND AND XKBCOMMON_X11_FOUND AND XCB_FOUND))
    message(FATAL_ERROR "Failed to find valid a display server: either x11 or wayland")
endif()