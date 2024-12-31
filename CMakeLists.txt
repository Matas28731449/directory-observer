cmake_minimum_required(VERSION 3.28)
project(directory_observer)

set(CMAKE_CXX_STANDARD 17)

add_library(directoryobserver EXCLUDE_FROM_ALL
        src/DirectoryObserver.cpp
)

target_include_directories(directoryobserver
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(tests
        tests/test_main.cpp
)

target_link_libraries(tests
        PRIVATE
        directoryobserver
)

add_executable(directory_observer
        main.cpp
)

target_link_libraries(directory_observer
        PRIVATE
        directoryobserver
)