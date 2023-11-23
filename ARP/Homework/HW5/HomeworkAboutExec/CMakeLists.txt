cmake_minimum_required(VERSION 3.2)

project("Week 5 homework")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

find_package(Curses REQUIRED)
find_library(<math.h> m)

include_directories("./headers")
set(WRAPSRC utility/wrapfunc.c)

add_executable(master master.c ${WRAPSRC})
add_executable(child child.c ${WRAPSRC})

target_link_libraries(master ${CURSES_LIBRARIES} m)
target_link_libraries(child ${CURSES_LIBRARIES} m)
