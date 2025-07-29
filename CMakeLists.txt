cmake_minimum_required(VERSION 3.20)

project(cmake_thread_async LANGUAGES CXX)

# C++ settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Final executable
add_executable(${PROJECT_NAME} src/main.cpp)