cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(main C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(main
    main.c
)
pico_add_extra_outputs(main)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)

target_link_libraries(main stdio pico_stdlib hardware_gpio hardware_timer hardware_dma hardware_pwm pico_binary_info)


