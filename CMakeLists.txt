cmake_minimum_required(VERSION 3.8)

set(C_STANDARD 99)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project("urlrouter")

include(GNUInstallDirs)

add_library(${PROJECT_NAME} STATIC src/urlrouter.c)

target_include_directories(${PROJECT_NAME} PUBLIC include)

install(TARGETS ${PROJECT_NAME} 
        EXPORT ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES include/urlrouter.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_executable(urlrouter_test src/test.c)
target_link_libraries(urlrouter_test PUBLIC ${PROJECT_NAME})

add_executable(urlrouter_example src/example.c)
target_link_libraries(urlrouter_example PUBLIC ${PROJECT_NAME})
