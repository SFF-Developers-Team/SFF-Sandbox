cmake_minimum_required(VERSION 3.5.0)
project(FriendsTeamSandBox)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

file(GLOB SOURCES
    "src/*.cpp"
    "src/entity/*.cpp"
    "src/world/blocks/*.cpp"
    "src/world/gen/*.cpp"
    "src/world/*.cpp"
    "src/particle/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(libs/raylib)

target_link_libraries(${PROJECT_NAME} raylib)

include_directories(
    "libs/raylib"
    "include/"
    "libs/perlin-noise"
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/)