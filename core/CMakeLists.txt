# SPDX-License-Identifier: MIT

if(DEFINED IDF_PATH)
    # ESP-IDF build
    idf_component_register(
        SRCS
            src/sx126x.c
        INCLUDE_DIRS include
    )
else()
    # Generic CMake build
    add_library(sx126x_core STATIC
        src/sx126x.c
    )

    target_include_directories(sx126x_core
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()
