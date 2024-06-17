cmake_minimum_required(VERSION 3.17)

add_executable(micokernel
        src/kmain.c
        inc/micok/rpc.h
        src/rpc.c
        inc/micok/emu.h
        src/emu.c)

target_include_directories(micokernel PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

target_link_libraries(micokernel PRIVATE micoshared)
target_compile_options(micokernel PRIVATE "-ffunction-sections" "-fomit-frame-pointer")
target_link_options(micokernel PRIVATE
        "-Wl,--entry=micoKMain"
        "-Wl,--default-script=${CMAKE_CURRENT_LIST_DIR}/kernel.ld"
        "-Wl,--orphan-handling=error")
add_custom_command(
        TARGET micokernel
        POST_BUILD
        COMMAND "${CMAKE_OBJCOPY}"
        ARGS
        -O binary
        "$<TARGET_FILE:micokernel>"
        "$<TARGET_FILE:micokernel>.bin"
)
