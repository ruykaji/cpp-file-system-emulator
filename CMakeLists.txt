cmake_minimum_required(VERSION 3.19.0)
project(cpp_file_system_emulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME}_lib SHARED src/file_system_emulator.cpp)
target_include_directories(${PROJECT_NAME}_lib PRIVATE include)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

find_package(GTest CONFIG REQUIRED)
if(NOT GTest_FOUND)
    message(WARNING "Google Test not found!")
else()
    include(GoogleTest)
    enable_testing()
    add_subdirectory(tests)
endif()