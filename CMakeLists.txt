cmake_minimum_required(VERSION 3.8)

set(C_STANDARD 99)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project("urlrouter")


add_executable(${PROJECT_NAME} src/main.c src/matcher.c)

target_include_directories(${PROJECT_NAME} PRIVATE include)




