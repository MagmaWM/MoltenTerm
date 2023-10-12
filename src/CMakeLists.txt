project(MagmaTerm)

set (SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/window.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window.c
    ${CMAKE_CURRENT_SOURCE_DIR}/magmaTerm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/magmaTerm.h

)

add_executable(moltenterm ${SOURCES})
target_link_libraries(moltenterm glfw glad)
