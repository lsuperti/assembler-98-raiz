
cmake_minimum_required(VERSION 3.12)
project(SO78044813 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ----------------------- || -------------

set(CMAKE_C_FLAGS_DEBUG "-g -rdynamic")
set(CMAKE_BUILD_TYPE Debug)

include_directories(${GTK3_INCLUDE_DIRS}) 
include_directories(${CMAKE_SOURCE_DIR}/include)

link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

file(GLOB SOURCES "src/*.c")

add_executable(A9R ${SOURCES})
target_link_libraries(A9R ${GTK3_LIBRARIES})

