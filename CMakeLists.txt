cmake_minimum_required(VERSION 3.27)
project(webmag)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(WEBMAG_INCLUDE_DIR     "${CMAKE_SOURCE_DIR}/include")
set(WEBMAG_SRC_DIR         "${CMAKE_SOURCE_DIR}/src")
set(WEBMAG_THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third_party")

set(GLFW_DIR        "${WEBMAG_THIRD_PARTY_DIR}/glfw")
set(WEBGPU_DIR      "${WEBMAG_THIRD_PARTY_DIR}/webgpu")
set(GLFW_WEBGPU_DIR "${WEBMAG_THIRD_PARTY_DIR}/glfw_webgpu")

include(warnings)

add_subdirectory(${GLFW_DIR})
add_subdirectory(${WEBGPU_DIR})
add_subdirectory(${GLFW_WEBGPU_DIR})
add_subdirectory(${WEBMAG_SRC_DIR})
