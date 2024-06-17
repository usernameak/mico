cmake_minimum_required(VERSION 3.17)

set(KERNEL_TOOLCHAIN_FILE_DIR "${CMAKE_CURRENT_LIST_DIR}")

if (NOT DEFINED KERNEL_COMPILER_ROOT_PATH)
    message(FATAL_ERROR "KERNEL_COMPILER_ROOT_PATH not specified. Point it to the root of arm-none-eabi-gcc installation.")
endif ()

list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES "KERNEL_COMPILER_ROOT_PATH")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_SYSROOT fs:/)
set(CMAKE_STAGING_PREFIX fs:/)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(CMAKE_C_COMPILER ${KERNEL_COMPILER_ROOT_PATH}/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${KERNEL_COMPILER_ROOT_PATH}/bin/arm-none-eabi-g++)
if (WIN32)
    set(CMAKE_C_COMPILER ${CMAKE_C_COMPILER}.exe)
    set(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER}.exe)
endif ()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_FLAGS "-fno-exceptions -marm -mthumb-interwork -march=armv6 -mtune=arm1136j-s -specs=nosys.specs -fno-pie -no-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-use-cxa-atexit -fno-rtti")
set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -static-libgcc -fno-pie -no-pie")
