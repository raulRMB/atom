cmake_minimum_required(VERSION 3.26)
project(atom)

file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/cpm.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
)
include (${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB_RECURSE SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

add_executable(atom ${SRC_FILES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        CXX_EXTENSIONS OFF
        COMPILE_WARNING_AS_ERROR ON
        VS_DEBUGGER_ENVIRONMENT "DAWN_DEBUG_BREAK_ON_ERROR=1"
)

if (MSVC)
    #target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

if(XCODE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            XCODE_GENERATE_SCHEME ON
            XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal")
endif()

set(DAWN_FETCH_DEPENDENCIES ON)
CPMAddPackage(
    NAME dawn 
    URL "https://dawn.googlesource.com/dawn/+archive/refs/heads/main.tar.gz"
)

CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 0.9.9.7
)

CPMAddPackage(
    NAME tiny_gltf
    GITHUB_REPOSITORY syoyo/tinygltf
    GIT_TAG v2.8.20
)

CPMAddPackage(
    NAME EnTT
    GITHUB_REPOSITORY skypjack/entt
    GIT_TAG v3.13.1
)

target_link_libraries(${PROJECT_NAME} PRIVATE webgpu_cpp webgpu_dawn webgpu_glfw glm tinygltf EnTT)
