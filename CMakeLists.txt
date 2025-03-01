cmake_minimum_required(VERSION 3.5.0)
project(SFFSandbox 
    DESCRIPTION "SFF Sandbox Game"
    HOMEPAGE_URL "https://github.com/Kolyah35/SFF-Sandbox"
    LANGUAGES C CXX
)

project(SFFSandbox-server)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(BUILD_SHARED_LIBS OFF)
set(ENET_STATIC ON CACHE BOOL "" FORCE)

set(SHARED_HEADERS 
    "libs/perlin-noise/"
    "libs/enet/inlcude"
    "libs/miniz/"
    "libs/toml/"
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
    "src/client/scenes/*.cpp"
    "src/client/ui/basic/*.cpp"
    "src/client/ui/gameplay/*.cpp"
)

file(GLOB SERVER_SOURCES CONFIGURE_DEPENDS
    "src/server/*.cpp"
)

add_subdirectory(libs/raylib)
add_subdirectory(libs/GitHash)
add_subdirectory(libs/miniz)
add_subdirectory(libs/enet)

if(${PLATFORM} STREQUAL "Android")
    set(PLATFORM_LIBS m c log android EGL GLESv2 OpenSLES dl unwind c++_static)
    add_library(SFFSandbox SHARED ${SHARED_SOURCES} ${CLIENT_SOURCES})
else()
    add_executable(SFFSandbox ${SHARED_SOURCES} ${CLIENT_SOURCES})
    add_executable(SFFSandbox-server ${SHARED_SOURCES} ${SERVER_SOURCES})
endif()

target_link_libraries(SFFSandbox raylib enet_static ${GITHASH_LIBRARIES} miniz ${PLATFORM_LIBS})

if(NOT ${PLATFORM} STREQUAL "Android")
    target_link_libraries(SFFSandbox-server enet_static ${GITHASH_LIBRARIES} miniz)
endif()

target_include_directories(SFFSandbox PRIVATE
    ${SHARED_HEADERS}
    "src/client/"
    "libs/raygui/src/"
    "libs/raylib/src/external/glfw/include/GLFW/"
)

if(NOT ${PLATFORM} STREQUAL "Android")
    target_include_directories(SFFSandbox-server PRIVATE
        ${SHARED_HEADERS}
        "src/server/"
    )
endif()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/)

if(${PLATFORM} STREQUAL "Android")
    set(VERSION_NUMBER 1)
    set(VERSION_NAME "1.0.0")

    set(OUT_DIR ${CMAKE_BINARY_DIR}/${OUT_DIR})
    file(MAKE_DIRECTORY ${OUT_DIR}/lib/${CMAKE_ANDROID_ARCH_ABI})
    file(MAKE_DIRECTORY ${OUT_DIR}/obj)
    file(MAKE_DIRECTORY ${OUT_DIR}/bin)
    file(COPY android/res DESTINATION ${OUT_DIR})
    configure_file(android/AndroidManifest.xml ${OUT_DIR}/AndroidManifest.xml)

    write_file(${OUT_DIR}/src/com/${DEV_NAME}/${PACKAGE_NAME}/NativeLoader.java 
        "package com.${DEV_NAME}.${PACKAGE_NAME};\n\n"
        "public class NativeLoader extends android.app.NativeActivity {\n"
        "   static {\n"
        "       System.loadLibrary(\"${LIB_NAME}\");\n"
        "   }\n"
        "}\n"
    )

    set(OUT_LIB_DIR ${OUT_DIR}/lib/${CMAKE_ANDROID_ARCH_ABI})
    set_target_properties(SFFSandbox PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUT_LIB_DIR} OUTPUT_NAME "${LIB_NAME}")
    add_compile_options(${ABI_FLAGS})

    # Fix java.lang.UnsatisfiedLinkError: dlopen failed: cannot locate symbol "_Unwind_Resume"
    add_compile_options(-ffunction-sections -funwind-tables -fstack-protector-strong -fPIC)
    add_compile_options(-Wa,--noexecstack -Wformat -Werror=format-security -Wunused-function -no-canonical-prefixes)
    target_link_options(SFFSandbox PUBLIC -Wl,--exclude-libs,libatomic.a -Wl,--build-id -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--warn-shared-textrel -Wl,--fatal-warnings -u ANativeActivity_onCreate)
endif()