cmake_minimum_required (VERSION 3.20)

project(nt5animation)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB LIBANIMATIONS_SRC CONFIGURE_DEPENDS "src/*.c")
add_library (${PROJECT_NAME} STATIC ${LIBANIMATIONS_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE
    "include"
)