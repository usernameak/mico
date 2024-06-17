cmake_minimum_required(VERSION 3.17)

add_library(micoshared STATIC
        inc/micos/emu.h
        inc/micos/xerr.h
        inc/micos/rpc.h
        inc/micos/memory.h
        src/memory/memutil.c)

target_include_directories(micoshared PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
