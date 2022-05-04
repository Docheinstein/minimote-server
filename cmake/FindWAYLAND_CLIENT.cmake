find_path(
        WAYLAND_CLIENT_INCLUDE_DIRS
        NAMES wayland-client.h
)

find_library(
        WAYLAND_CLIENT_LIBRARIES
        NAMES wayland-client
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
        WAYLAND_CLIENT
        DEFAULT_MSG
        WAYLAND_CLIENT_LIBRARIES
        WAYLAND_CLIENT_INCLUDE_DIRS
)