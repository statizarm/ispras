cmake_minimum_required(VERSION 3.20)
project(yadpll)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES include/literal.h src/literal.cpp include/cnf.h src/cnf.cpp)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

enable_testing()

add_executable(unit ${SOURCES} ./tests/unit.cpp)

target_include_directories(
        unit PRIVATE ./include
        ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(unit ${GTEST_LIBRARIES} gtest gtest_main)

add_executable(stress ${SOURCES} tests/stress.cpp)

target_include_directories(
        stress PRIVATE ./include
        ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(stress ${GTEST_LIBRARIES} gtest gtest_main)

add_executable(yadpll main.cpp ${SOURCES})

target_include_directories(yadpll PRIVATE include/)
