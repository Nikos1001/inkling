
cmake_minimum_required(VERSION 3.5)

set(INSTALL_PATH "$ENV{HOME}/.inkling")
file(MAKE_DIRECTORY ${INSTALL_PATH})

set(INKLIB_UTIL_SRCS inklib/src/memory.c inklib/src/arena.c inklib/src/str.c inklib/src/file.c inklib/src/math.c inklib/src/reflect.c inklib/src/respool.c inklib/src/gfxTypeInfos.c)

project(inklib)

add_library(inklib STATIC ${INKLIB_UTIL_SRCS} inklib/src/window.c inklib/src/gfx.c inklib/src/log.c inklib/src/input.c)
target_include_directories(inklib PRIVATE inklib/include/)

find_package(GLEW 2.0 REQUIRED)
target_link_libraries(inklib GLEW::GLEW)
target_include_directories(inklib PRIVATE ${GLEW_INCLUDE})

find_package(glfw3 REQUIRED)
target_link_libraries(inklib glfw)
target_include_directories(inklib PRIVATE ${GLFW_INCLUDE})

install(TARGETS inklib DESTINATION ${INSTALL_PATH}/lib)
install(DIRECTORY inklib/include/inkling DESTINATION ${INSTALL_PATH}/include)



project(inklib-dev)

add_library(inklib-dev STATIC ${INKLIB_UTIL_SRCS} inklib/dev/api.c)
target_include_directories(inklib-dev PRIVATE inklib/include/)

install(TARGETS inklib-dev DESTINATION ${INSTALL_PATH}/lib)



project(inkdev)

add_executable(inkdev inkdev/main.c inkdev/dll.c)
target_include_directories(inkdev PRIVATE inklib/include)
target_link_libraries(inkdev inklib)
