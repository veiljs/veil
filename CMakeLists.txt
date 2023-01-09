cmake_minimum_required(VERSION 3.10)

project(veil LANGUAGES C)
set(CMAKE_C_STANDARD 11)

set(LIBUV_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
add_subdirectory(ext/libuv/repo EXCLUDE_FROM_ALL)

add_subdirectory(ext/quickjs EXCLUDE_FROM_ALL)

add_subdirectory(ext/stc EXCLUDE_FROM_ALL)

add_executable(veil
    src/main.c
    src/veil.c
    src/util.c
    src/vm.c
    src/uv.c
    src/cfg.c
)

target_include_directories(veil
    PRIVATE
    inc
    ext/libuv/repo/include
    ext/quickjs/repo
    ext/stc/repo/include
)

target_link_libraries(veil
    PRIVATE
    qjs_a
    uv_a
    stc_a
    m
    pthread
    dl
)
