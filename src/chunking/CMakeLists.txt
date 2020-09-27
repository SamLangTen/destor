SET(libchunk_a_SOURCES rabin_chunking.c ae_chunking.c)

find_package (PkgConfig REQUIRED)
pkg_check_modules (GLIB2 glib-2.0)

include_directories (${GLIB2_INCLUDE_DIRS})

add_library (libchunk STATIC ${libchunk_a_SOURCES})