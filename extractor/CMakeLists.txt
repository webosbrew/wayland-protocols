find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)

add_executable(wl-extractor main.c interfaces.c output.c)
target_include_directories(wl-extractor PRIVATE ${WAYLAND_CLIENT_INCLUDE_DIRS})
target_link_libraries(wl-extractor PRIVATE dl)
target_compile_definitions(wl-extractor PRIVATE _GNU_SOURCE)