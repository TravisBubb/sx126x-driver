# SPDX-License-Identifier: MIT

add_executable(minimal_build
    main.c
)

target_link_libraries(minimal_build
    sx126x_core
    sx126x_hal_esp32
)