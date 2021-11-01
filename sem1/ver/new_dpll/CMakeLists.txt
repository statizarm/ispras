cmake_minimum_required(VERSION 3.20)
project(yadpll)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES include/clause.h src/clause.cpp include/cnf.h src/cnf.cpp src/dpll.cpp include/dpll.h)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

enable_testing()

add_executable(tests ${SOURCES} tests/main.cpp)

target_include_directories(
        tests PRIVATE ./include
        ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(tests ${GTEST_LIBRARIES} gtest gtest_main)
# link_libraries(${GTEST_LIBRARIES} gtest gtest_main)

add_test(tests ./tests)

add_executable(yadpll main.cpp include/clause.h src/clause.cpp include/cnf.h src/cnf.cpp src/dpll.cpp include/dpll.h)

target_include_directories(yadpll PRIVATE include/)
