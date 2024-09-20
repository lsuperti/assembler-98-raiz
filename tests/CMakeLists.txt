
cmake_minimum_required(VERSION 3.12)
project(SO78044813 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ----------------------- || -------------

file(GLOB MONTADOR_SOURCES 
    ../src/montador.c
    ../src/parser.c
    ../src/global.c
    ../src/helper.c
    ../src/data_structures.c
    ../src/architecture.c
)
file(GLOB LIGADOR_SOURCES  ../src/helper.c)
file(GLOB PROCESSADOR_MACROS_SOURCES 
    ../src/macro_processor.c
    ../src/montador.c
    ../src/global.c
    ../src/parser.c
    ../src/helper.c
    ../src/data_structures.c
    ../src/architecture.c
)

set(CMAKE_C_FLAGS_DEBUG "-g")

file(GLOB_RECURSE MONTADOR_TEST_SOURCES montador/*.c)
file(GLOB_RECURSE LIGADOR_TEST_SOURCES ligador/*.c)
file(GLOB_RECURSE MACRO_TEST_SOURCES processador_macros/*.c)

include_directories(${GTK3_INCLUDE_DIRS}) 
include_directories(../include)

link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(montador_suite ${MONTADOR_SOURCES} ${MONTADOR_TEST_SOURCES} )
add_executable(ligador_suite ${LIGADOR_SOURCES} ${LIGADOR_TEST_SOURCES} )
add_executable(macros_suite ${PROCESSADOR_MACROS_SOURCES} ${MACRO_TEST_SOURCES} )

target_link_libraries(montador_suite ${GTK3_LIBRARIES} criterion )
target_link_libraries(ligador_suite ${GTK3_LIBRARIES} criterion )
target_link_libraries(macros_suite ${GTK3_LIBRARIES} criterion )

