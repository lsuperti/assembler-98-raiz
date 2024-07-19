
cmake_minimum_required(VERSION 3.20)
project(SO78044813 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# ----------------------- || -------------

include_directories(${GTK4_INCLUDE_DIRS}) 
include_directories(${CMAKE_SOURCE_DIR}/include)

link_directories(${GTK4_LIBRARY_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})

file(GLOB SOURCES "src/*.c")

add_executable(A9R ${SOURCES})
target_link_libraries(A9R ${GTK4_LIBRARIES})

