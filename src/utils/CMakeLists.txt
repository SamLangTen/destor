SET(libutils_a_SOURCES lru_cache.c sync_queue.c queue.c serial.c bloom_filter.c sds.c)

find_package (PkgConfig REQUIRED)
pkg_check_modules (GLIB2 glib-2.0)

include_directories (${GLIB2_INCLUDE_DIRS})

add_library (libutils STATIC ${libutils_a_SOURCES})