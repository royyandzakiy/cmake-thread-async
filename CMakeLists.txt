cmake_minimum_required(VERSION 3.20)

project(cmake_thread_async LANGUAGES CXX)

# Use VCPKG toolchain (set externally or via presets)
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    message(STATUS "VCPKG_ROOT: $ENV{VCPKG_ROOT}")
    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
endif()

# C++ settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies (resolved through vcpkg)
find_package(fmt CONFIG REQUIRED)

# Final executable
add_executable(${PROJECT_NAME} src/main.cpp)