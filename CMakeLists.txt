cmake_minimum_required(VERSION 3.5.0)
project(SFFSandbox)
project(SFFSandbox-server)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(BUILD_SHARED_LIBS OFF)

set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RENAME_ZCONF OFF CACHE BOOL "" FORCE)
set(ENET_STATIC ON CACHE BOOL "" FORCE)

set(SHARED_HEADERS 
    "libs/perlin-noise/"
    "libs/enet/inlcude"
    "libs/toml/"
    "libs/enet/include"
    "src/shared/"
)

file(GLOB SHARED_SOURCES CONFIGURE_DEPENDS
    "src/shared/blocks/*.cpp"
    "src/shared/entity/*.cpp"
    "src/shared/world/gen/*.cpp"
    "src/shared/world/*.cpp"
    "src/shared/*.cpp"
)

file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS
    "src/client/*.cpp"
    "src/client/entity/*.cpp"
    "src/client/particle/*.cpp"
    "src/client/ui/*.cpp"
    "src/client/ui/nodes/*.cpp"
)

file(GLOB SERVER_SOURCES CONFIGURE_DEPENDS
    "src/server/*.cpp"
)

add_subdirectory(libs/raylib)
add_subdirectory(libs/GitHash)
add_subdirectory(libs/zlib)
add_subdirectory(libs/enet)

add_executable(SFFSandbox ${SHARED_SOURCES} ${CLIENT_SOURCES})
add_executable(SFFSandbox-server ${SHARED_SOURCES} ${SERVER_SOURCES})


target_link_libraries(SFFSandbox raylib enet_static ${GITHASH_LIBRARIES} zlibstatic)
target_link_libraries(SFFSandbox-server enet_static ${GITHASH_LIBRARIES} zlibstatic)

target_include_directories(SFFSandbox PRIVATE
    ${SHARED_HEADERS}
    "src/client/"
    "libs/raygui/src/"
    "libs/raylib/src/external/glfw/include/GLFW/"
)

target_include_directories(SFFSandbox-server PRIVATE
    ${SHARED_HEADERS}
    "src/server/"
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/)