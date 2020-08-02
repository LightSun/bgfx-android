cmake_minimum_required(VERSION 3.4.1)

#gles
#如果当前系统版本过低设置拒绝编译
if (${ANDROID_PLATFORM_LEVEL} LESS 11)
    message(FATAL_ERROR "OpenGL 2 is not supported before API level 11 (currently using ${ANDROID_PLATFORM_LEVEL}).")
    return()
elseif (${ANDROID_PLATFORM_LEVEL} LESS 18)
    add_definitions("-DDYNAMIC_ES3")
    set(OPENGL_LIB GLESv2)
else ()
    set(OPENGL_LIB GLESv3)
endif (${ANDROID_PLATFORM_LEVEL} LESS 11)

include_directories(src/main/cpp/include)
#include_directories(src/main/cpp/thirds)
#include_directories(src/main/cpp/src)

set(LIB_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI})
add_library(bgfx STATIC IMPORTED)
set_target_properties(bgfx PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libbgfxDebug.a)

add_library(bimg_decode STATIC IMPORTED)
set_target_properties(bimg_decode PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libbimg_decodeDebug.a)

add_library(bimg STATIC IMPORTED)
set_target_properties(bimg PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libbimgDebug.a)

add_library(bx STATIC IMPORTED)
set_target_properties(bx PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libbxDebug.a)

aux_source_directory(src/main/cpp/src SRC_study)
#aux_source_directory(src/main/cpp/src/common SRC_common)
#aux_source_directory(src/main/cpp/src/common/entry SRC_entry)
#aux_source_directory(src/main/cpp/src/common/imgui SRC_imgui)

#aux_source_directory(src/main/cpp/src/thirds/dear-imgui SRC_dear_imgui)
#aux_source_directory(src/main/cpp/src/thirds/meshoptimizer SRC_meshoptimizer)
#aux_source_directory(src/main/cpp/src/thirds/iconfontheaders SRC_iconfontheaders)


add_library(
        bgfx_study
        SHARED
        ${SRC_study}
)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

target_link_libraries(
        bgfx_study
        bgfx
        bimg_decode
        bimg
        bx
        ${OPENGL_LIB}
        android
        EGL
        ${log-lib}
)