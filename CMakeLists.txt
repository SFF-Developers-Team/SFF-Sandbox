cmake_minimum_required(VERSION 3.10)

include(cmake/CPM.cmake)

project(SFFSandbox 
    DESCRIPTION "SFF Sandbox Game"
    HOMEPAGE_URL "https://github.com/Kolyah35/SFF-Sandbox"
    LANGUAGES C CXX
)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(BUILD_SHARED_LIBS OFF)

CPMAddPackage("gh:raysan5/raylib#5.5")
CPMAddPackage("gh:RobLoach/raylib-cpp@5.5.0")
CPMAddPackage("gh:google/googletest@1.17.0")
CPMAddPackage("gh:Reputeless/PerlinNoise@3.0.0")

set(SHARED_HEADERS 
    "${raylib_cpp_SOURCE_DIR}/src/"
    "${CMAKE_SOURCE_DIR}/src/shared/"
    "${PerlinNoise_SOURCE_DIR}"
)

file(GLOB SHARED_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/shared/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/shared/worldgen/*.cpp"
)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/tests/*.cpp"
)

file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS
    "src/client/*.cpp"
)

file(GLOB SERVER_SOURCES CONFIGURE_DEPENDS
    "src/server/main.cpp"
    "src/server/*.cpp"
)

add_executable(${PROJECT_NAME}-tests ${SHARED_SOURCES} ${TEST_SOURCES})
target_include_directories(${PROJECT_NAME}-tests PUBLIC ${SHARED_HEADERS})
target_link_libraries(${PROJECT_NAME}-tests gtest pthread raylib_cpp)

enable_testing()
add_test(NAME "test-all" COMMAND ${PROJECT_NAME}-tests)

if(${PLATFORM} STREQUAL "Android")
    set(PLATFORM_LIBS m c log android EGL GLESv2 OpenSLES dl unwind c++_static)
    add_library(${PROJECT_NAME} SHARED ${SHARED_SOURCES} ${CLIENT_SOURCES})
else()
    add_executable(${PROJECT_NAME} ${SHARED_SOURCES} ${CLIENT_SOURCES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SHARED_HEADERS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DSANDBOX_CLIENT -DSUPPORT_IMAGE_GENERATION=0)
    # add_executable(${PROJECT_NAME}-server ${SHARED_SOURCES} ${SERVER_SOURCES})
endif()

target_link_libraries(${PROJECT_NAME} raylib raylib_cpp ${PLATFORM_LIBS})

# if(NOT ${PLATFORM} STREQUAL "Android")
#     target_link_libraries(${PROJECT_NAME}-server enet_static ${GITHASH_LIBRARIES} miniz)
# endif()

# target_include_directories(${PROJECT_NAME} PRIVATE
#     ${SHARED_HEADERS}
#     "src/client/"
#     "libs/raygui/src/"
#     "libs/raylib/src/external/glfw/include/GLFW/"
# )

# if(NOT ${PLATFORM} STREQUAL "Android")
#     target_include_directories(${PROJECT_NAME}-server PRIVATE
#         ${SHARED_HEADERS}
#         "src/server/"
#     )
# endif()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/)

# if(${PLATFORM} STREQUAL "Android")
#     set(VERSION_NUMBER 1)
#     set(VERSION_NAME "1.0.0")

#     set(OUT_DIR ${CMAKE_BINARY_DIR}/${OUT_DIR})
#     file(MAKE_DIRECTORY ${OUT_DIR}/lib/${CMAKE_ANDROID_ARCH_ABI})
#     file(MAKE_DIRECTORY ${OUT_DIR}/obj)
#     file(MAKE_DIRECTORY ${OUT_DIR}/bin)
#     file(COPY android/res DESTINATION ${OUT_DIR})
#     configure_file(android/AndroidManifest.xml ${OUT_DIR}/AndroidManifest.xml)

#     write_file(${OUT_DIR}/src/com/${DEV_NAME}/${PACKAGE_NAME}/NativeLoader.java 
#         "package com.${DEV_NAME}.${PACKAGE_NAME};\n\n"
#         "public class NativeLoader extends android.app.NativeActivity {\n"
#         "   static {\n"
#         "       System.loadLibrary(\"${LIB_NAME}\");\n"
#         "   }\n"
#         "}\n"
#     )

#     set(OUT_LIB_DIR ${OUT_DIR}/lib/${CMAKE_ANDROID_ARCH_ABI})
#     set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUT_LIB_DIR} OUTPUT_NAME "${LIB_NAME}")
#     add_compile_options(${ABI_FLAGS})

#     # Fix java.lang.UnsatisfiedLinkError: dlopen failed: cannot locate symbol "_Unwind_Resume"
#     add_compile_options(-ffunction-sections -funwind-tables -fstack-protector-strong -fPIC)
#     add_compile_options(-Wa,--noexecstack -Wformat -Werror=format-security -Wunused-function -no-canonical-prefixes)
#     target_link_options(${PROJECT_NAME} PUBLIC -Wl,--exclude-libs,libatomic.a -Wl,--build-id -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--warn-shared-textrel -Wl,--fatal-warnings -u ANativeActivity_onCreate)
# endif()