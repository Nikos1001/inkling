
cmake_minimum_required(VERSION 3.5)

set(INSTALL_PATH "$ENV{HOME}/.inkling")
file(MAKE_DIRECTORY ${INSTALL_PATH})

set(INKLIB_UTIL_SRCS inklib/src/memory.c inklib/src/arena.c inklib/src/str.c inklib/src/file.c inklib/src/math.c inklib/src/reflect.c inklib/src/respool.c inklib/src/gfxTypeInfos.c)

project(inklib)

add_library(inklib STATIC ${INKLIB_UTIL_SRCS} inklib/src/window.c inklib/src/gfx.c inklib/src/log.c inklib/src/input.c)
target_include_directories(inklib PRIVATE inklib/include/)

add_definitions(-DGLEW_STATIC)
add_subdirectory(libs/glew EXCLUDE_FROM_ALL)
target_link_libraries(inklib PUBLIC libglew_static)
target_include_directories(inklib PRIVATE libs/glew/include/)
install(TARGETS libglew_static DESTINATION ${INSTALL_PATH}/lib)

add_subdirectory(libs/glfw EXCLUDE_FROM_ALL)
target_link_libraries(inklib PUBLIC glfw)
target_include_directories(inklib PRIVATE libs/glfw/include/)
install(TARGETS glfw DESTINATION ${INSTALL_PATH}/lib)

install(TARGETS inklib DESTINATION ${INSTALL_PATH}/lib)
install(DIRECTORY inklib/include/inkling DESTINATION ${INSTALL_PATH}/include)



project(inklib-dev)

add_library(inklib-dev STATIC ${INKLIB_UTIL_SRCS} inklib/dev/api.c)
target_include_directories(inklib-dev PRIVATE inklib/include/)

install(TARGETS inklib-dev DESTINATION ${INSTALL_PATH}/lib)



project(inkrun)

add_library(inkrun STATIC inkrun/entry.c)
target_include_directories(inkrun PRIVATE inklib/include/)
target_link_libraries(inkrun PUBLIC inklib)

install(TARGETS inkrun DESTINATION ${INSTALL_PATH}/lib)



project(inkdev)

add_executable(inkdev inkdev/main.c inkdev/dll.c inkdev/build.c)
target_include_directories(inkdev PRIVATE inklib/include)
target_link_libraries(inkdev inklib)


