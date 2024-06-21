cmake_minimum_required(VERSION 3.17)

find_path(UNICORN_INCLUDE_DIRS unicorn/unicorn.h)
find_library(UNICORN_LIBRARIES unicorn)

add_executable(micoclient main.c
        memory/hostmem.h
        display/dpy_win.c
        display/dpy.h
        emu/emu.c
        emu/emu.h
        emu/device.h
        emu/loader.c
        emu/loader.h
        display/dpydev.h
        display/dpydev.c
        os/event.c
        os/event.h
)

if(WIN32)
    target_sources(micoclient PRIVATE memory/hostmem_win.c)
endif()

target_link_libraries(micoclient PRIVATE ${UNICORN_LIBRARIES} micoshared)
target_include_directories(micoclient PRIVATE ${UNICORN_INCLUDE_DIRS})
target_compile_definitions(micoclient PRIVATE _CRT_SECURE_NO_WARNINGS)
