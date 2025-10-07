# SPDX-License-Identifier: MIT

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_HAL_ESP32 "Build ESP32 HAL" OFF)
option(BUILD_EXAMPLES "Build example programs" OFF)

message(STATUS "BUILD_HAL_ESP32 = ${BUILD_HAL_ESP32}")
message(STATUS "BUILD_EXAMPLES = ${BUILD_EXAMPLES}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Detect if building under ESP-IDF
if(DEFINED IDF_TARGET)
    message(STATUS "Building SX126x driver as EDP-IDF component")

    idf_component_register(
        SRCS
            core/src/sx126x.c
        INCLUDE_DIRS
            core/include
            hal/esp32/include
    )
else()
    cmake_minimum_required(VERSION 3.16)
    project(sx126x_driver C)

    add_subdirectory(core)

    if (BUILD_HAL_ESP32)
        add_subdirectory(hal/esp32)
    endif()

    if (BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()