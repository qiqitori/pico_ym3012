cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(ym3012_dac C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

include(example_auto_set_url.cmake)

add_compile_options(-Wall -Wextra
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -O4
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(ym3012_dac
               ym3012_dac.c
               )

target_link_libraries(ym3012_dac PRIVATE
        pico_stdlib
        pico_multicore
        hardware_pio
        hardware_pwm
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(ym3012_dac 1)
pico_enable_stdio_uart(ym3012_dac 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ym3012_dac)

# add url via pico_set_program_url
example_auto_set_url(ym3012_dac)

pico_generate_pio_header(ym3012_dac ${CMAKE_CURRENT_LIST_DIR}/ym3012_dac.pio)

target_sources(ym3012_dac PRIVATE ym3012_dac.c)

pico_add_extra_outputs(ym3012_dac)
