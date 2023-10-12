project (Glad)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/include/glad/gl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/khrplatform.h
)

add_library(glad STATIC ${SOURCES})
target_include_directories(glad PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include)