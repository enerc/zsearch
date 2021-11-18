cmake_minimum_required (VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-march=native -DNDEBUG -g -O3")
SET(ASM_OPTIONS "-x assembler-with-cpp")
enable_language(ASM)


project (zsearch CXX)
find_package(Threads REQUIRED)
find_path(MHD_INCLUDE_DIR NAMES microhttpd.h)
find_library(MHD_LIBRARY microhttpd)
if(NOT MHD_LIBRARY)
    message(FATAL_ERROR "libmicrohttpd library not found")
endif()

set(PRE_CONFIGURE_FILE "git.h.in")
set(POST_CONFIGURE_FILE "git.h")
include(tools/git_watcher.cmake)

include(src/common/CMakeLists.txt)
include(src/repository/CMakeLists.txt)
include(src/dictionary/CMakeLists.txt)
include(src/mapping/CMakeLists.txt)
include(src/mapping/CMakeLists.txt)
include(src/devices/CMakeLists.txt)
include(src/queries/CMakeLists.txt)
include(src/sql/CMakeLists.txt)
include(src/tools/CMakeLists.txt)
include(tests/CMakeLists.txt)

list(APPEND zearch_sources
)
add_executable(zsearch src/zsearch.cpp ${zsearch_sources})
add_dependencies(zsearch check_git)

target_link_libraries(zsearch -pthread -llz4 -ltbb -lmicrohttpd -lfmt)

