cmake_minimum_required(VERSION 3.17)
project(mico C ASM)

set(CMAKE_C_STANDARD 99)

option(BUILD_CLIENT "Build micoclient" ON)

add_subdirectory(shared)
if (BUILD_CLIENT)
    add_subdirectory(client)
else ()
    add_subdirectory(kernel)
endif ()
