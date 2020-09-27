SET(libdestor_a_SOURCES destor.c jcr.c config.c do_backup.c read_phase.c chunk_phase.c hash_phase.c trace_phase.c dedup_phase.c rewrite_phase.c filter_phase.c cfl_rewrite.c cap_rewrite.c cbr_rewrite.c har_rewrite.c restore_aware.c do_restore.c optimal_restore.c assembly_restore.c cma.c do_delete.c)
add_subdirectory(chunking)
add_subdirectory(fsl)
add_subdirectory(index)
add_subdirectory(recipe)
add_subdirectory(storage)
add_subdirectory(utils)

find_package (PkgConfig REQUIRED)
pkg_check_modules (GLIB2 glib-2.0)

include_directories (${GLIB2_INCLUDE_DIRS})

add_library (libdestor STATIC ${libdestor_a_SOURCES})

target_link_libraries(libdestor libchunk libfsl libindex librecipe libstorage libutils)