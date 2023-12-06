cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(sensor C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(sensor
    sensor.c
)

target_link_libraries(sensor pico_stdlib hardware_gpio hardware_timer hardware_dma)

pico_add_extra_outputs(sensor)
