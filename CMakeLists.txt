cmake_minimum_required(VERSION 3.23)
project(MagmaTerm)
set(CMAKE_C_STANDARD 99)
option(BUNDLE_GLFW "Build the bundled GLFW library and link it to the final executable" ON)

add_subdirectory(glad)

if(BUNDLE_GLFW)
    add_subdirectory(./glfw)
else()
    find_library(GLFW_LIB_NAME glfw glfw3 REQUIRED)
endif()

add_subdirectory(./src)