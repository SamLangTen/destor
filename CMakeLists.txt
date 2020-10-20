cmake_minimum_required (VERSION 3.1)


project (destor)

SET(CMAKE_BUILD_TYPE "Debug")  
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")  

add_subdirectory (src)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 glib-2.0)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
if (USE_SQLITE)
    find_package(SQLite3 REQUIRED)
endif (USE_SQLITE)
if (USE_MYSQL)
    pkg_check_modules(MYSQL libmariadb)
endif (USE_MYSQL)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/destor.config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/rebuild DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories (${GLIB2_INCLUDE_DIRS})

add_executable(destor ./src/destor.c)
set_property(TARGET destor PROPERTY C_STANDARD 11)

set(LIB_LINK libdestor ${GLIB2_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(destor ${LIB_LINK})
