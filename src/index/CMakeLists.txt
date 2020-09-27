set(libindex_a_SOURCES index.c fingerprint_cache.c kvstore.c kvstore_htable.c sampling_method.c segmenting_method.c similarity_detection.c)

find_package (PkgConfig REQUIRED)
pkg_check_modules (GLIB2 glib-2.0)

include_directories (${GLIB2_INCLUDE_DIRS})

add_library (libindex STATIC ${libindex_a_SOURCES})