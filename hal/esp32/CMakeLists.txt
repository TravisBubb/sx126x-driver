# SPDX-License-Identifier: MIT

if(DEFINED IDF_PATH)
    # Building under ESP-IDF
    idf_component_register(
        SRCS "src/sx126x_hal_esp32.c"
        INCLUDE_DIRS "include"
        REQUIRES driver spi
    )
else()
    add_library(sx126x_hal_esp32 STATIC
        src/sx126x_hal_esp32.c
    )

    target_include_directories(sx126x_hal_esp32
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_link_libraries(sx126x_hal_esp32 PRIVATE sx126x_core)
endif()