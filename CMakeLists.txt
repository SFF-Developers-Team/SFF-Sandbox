cmake_minimum_required(VERSION 3.5.0)
project(SFFSandbox)
project(SFFSandbox-server)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(SOCKPP_BUILD_EXAMPLES ON)

set(SHARED_HEADERS 
    "libs/perlin-noise"
    "libs/sockpp/include"

    "src/shared/blocks/"
    "src/shared/entity/"
    "src/shared/world/gen/"
    "src/shared/world/"
    "src/shared/"
)

file(GLOB SHARED_SOURCES 
    "src/shared/blocks/*.cpp"
    "src/shared/entity/*.cpp"
    "src/shared/world/gen/*.cpp"
    "src/shared/world/*.cpp"
    "src/shared/*.cpp"
)

file(GLOB CLIENT_SOURCES
    "src/client/*.cpp"
    "src/client/entity/*.cpp"
    "src/client/particle/*.cpp"
)

file(GLOB SERVER_SOURCES
    "src/server/*.cpp"
)

add_executable(SFFSandbox ${SHARED_SOURCES} ${CLIENT_SOURCES})
add_executable(SFFSandbox-server ${SHARED_SOURCES} ${SERVER_SOURCES})

add_subdirectory(libs/sockpp)
add_subdirectory(libs/raylib)
add_subdirectory(libs/GitHash)

target_link_libraries(SFFSandbox raylib sockpp ${GITHASH_LIBRARIES})
target_link_libraries(SFFSandbox-server sockpp ${GITHASH_LIBRARIES})

target_include_directories(SFFSandbox PRIVATE
    ${SHARED_HEADERS}
    "src/client/entity"
    "src/client/particle"
    "src/client/world/gen"
    "src/client/world"
    "src/client/"
)

target_include_directories(SFFSandbox-server PRIVATE
    ${SHARED_HEADERS}
    "libs/toml/"
    "src/server/world/gen"
    "src/server/world"
    "src/server/"
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/)