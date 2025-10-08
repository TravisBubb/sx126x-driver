# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Optional: examples only
option(BUILD_EXAMPLES "Build example programs" OFF)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# --- ESP-IDF component mode ---
if (DEFINED IDF_TARGET)
    message(STATUS "Building SX126x driver as ESP-IDF component")

    idf_component_register(
        SRCS
            core/src/sx126x.c
            hal/esp32/src/sx126x_hal_esp32.c
        INCLUDE_DIRS
            core/include
            hal/esp32/include
        REQUIRES
            driver
    )

# --- Standalone mode (non-ESP-IDF) ---
else()
    project(sx126x_driver C)

    add_library(sx126x_driver STATIC
        core/src/sx126x.c
        hal/esp32/src/sx126x_hal_esp32.c
    )

    target_include_directories(sx126x_driver PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/core/include
        ${CMAKE_CURRENT_SOURCE_DIR}/hal/esp32/include
    )
endif()
