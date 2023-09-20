add_executable(wayland_egl wayland_egl.c wayland_compat.c)

find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/webos-shell-client-protocol.h
        COMMAND ${WAYLAND_SCANNER} client-header ${CMAKE_CURRENT_SOURCE_DIR}/../protocols/merged/tv-1.2.0/webos-shell.xml
        ${CMAKE_CURRENT_BINARY_DIR}/webos-shell-client-protocol.h
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../protocols/merged/tv-1.2.0/webos-shell.xml)

target_sources(wayland_egl PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/webos-shell-client-protocol.h)

target_include_directories(wayland_egl PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

find_package(PkgConfig REQUIRED)

pkg_check_modules(WLCLIENT REQUIRED wayland-webos-client)
target_include_directories(wayland_egl SYSTEM PRIVATE ${WLCLIENT_INCLUDE_DIRS})

pkg_check_modules(WLEGL REQUIRED wayland-egl)
target_include_directories(wayland_egl SYSTEM PRIVATE ${WLEGL_INCLUDE_DIRS})

pkg_check_modules(GLESV2 REQUIRED glesv2)
target_include_directories(wayland_egl SYSTEM PRIVATE ${GLESV2_INCLUDE_DIRS})

target_link_libraries(wayland_egl ${WLCLIENT_LDFLAGS} ${WLEGL_LDFLAGS} -lEGL ${GLESV2_LDFLAGS})