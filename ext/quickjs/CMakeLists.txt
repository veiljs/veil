project(quickjs LANGUAGES C)

add_library(qjs_a
    STATIC
    repo/cutils.c
    repo/libbf.c
    repo/libregexp.c
    repo/libunicode.c
    repo/quickjs.c
)

target_compile_options(qjs_a 
    PRIVATE
    -funsigned-char
    -Wall
    -Wextra
    -Wno-array-bounds
    -Wno-missing-field-initializers
    -Wno-sign-compare
    -Wno-unused-but-set-variable
    -Wno-unused-parameter
    -Wno-unused-variable
)

file(STRINGS "repo/VERSION" QJS_VERSION)
target_compile_definitions(qjs_a
    PRIVATE
    CONFIG_BIGNUM
    CONFIG_VERSION="${QJS_VERSION}"
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(qjs_a 
      PRIVATE
      DUMP_LEAKS
  )
endif()

target_include_directories(qjs_a
    PUBLIC
    repo/src
)
