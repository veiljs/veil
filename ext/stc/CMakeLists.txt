project(stc LANGUAGES C)

add_library(stc_a
    STATIC
    repo/src/libstc.c
)

target_include_directories(stc_a
    PRIVATE
    repo/include
)
