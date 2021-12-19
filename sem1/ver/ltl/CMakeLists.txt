cmake_minimum_required(VERSION 3.20)
project(ltl)

set(CMAKE_CXX_STANDARD 17)

set(LIB_SOURCES lib/src/ltl.cpp)
set(LIB_INCLUDES lib/include/fsm.h lib/include/ltl.h)

set(SOURCES src/compiler.cpp src/atom.cpp)
set(INCLUDES include/compiler.h include/atom.h)

add_executable(ltl src/main.cpp ${LIB_SOURCES} ${LIB_INCLUDES} ${SOURCES} ${INCLUDES})
add_executable(test lib/src/test.cpp ${LIB_SOURCES} ${LIB_INCLUDES})

target_include_directories(ltl PRIVATE include/ PRIVATE lib/include/)
target_include_directories(test PRIVATE lib/include/)
