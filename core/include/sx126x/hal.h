// SPDX-License-Identifier: MIT

/**
 * @file hal.h
 * @brief Public API for SX126x LoRa HAL.
 * @version 0.1
 * @date 2025
 */

#ifndef SX126X_HAL_H
#define SX126X_HAL_H

#include "sx126x/bus.h"
#include "sx126x/sx126x.h"
#include "sx126x/types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Opaque handle for platform-specific HAL state.
 *
 * Should be implemented in each platform-specific HAL.
 */
typedef struct sx126x_hal_s sx126x_hal_t;

sx126x_bus_t *sx126x_hal_get_bus(sx126x_hal_t *hal);
sx126x_t *sx126x_hal_get_device(sx126x_hal_t *hal);

#ifdef __cplusplus
}
#endif

#endif // SX126X_HAL_H