SET(libfsl_a_SOURCES libhashfile.c read_fsl_trace.c)

find_package (PkgConfig REQUIRED)
pkg_check_modules (GLIB2 glib-2.0)

include_directories (${GLIB2_INCLUDE_DIRS})

add_library (libfsl STATIC ${libfsl_a_SOURCES})