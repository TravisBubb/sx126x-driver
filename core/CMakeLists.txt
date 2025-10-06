# SPDX-License-Identifier: MIT

add_library(sx126x_core STATIC 
    src/sx126x.c
)

target_include_directories(sx126x_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)