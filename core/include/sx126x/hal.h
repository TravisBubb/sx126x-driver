// SPDX-License-Identifier: MIT

/**
 * @file sx126x_hal.h
 * @brief Public API for SX126x LoRa HAL.
 * @version 0.1
 * @date 2025
 */

#ifndef SX126X_HAL_H
#define SX126X_HAL_H

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

/**
 * @brief Represents a HAL instance; Contains function pointers and platform-specific state.
 */
struct sx126x_hal_s
{
  sx126x_status_t (*init)(struct sx126x_hal_s *hal, const void *cfg);
  sx126x_status_t (*deinit)(struct sx126x_hal_s *hal);
  sx126x_status_t (*cmd_write)(struct sx126x_hal_s *hal, uint8_t opcode, const uint8_t *data, size_t len);
  sx126x_status_t (*cmd_read)(struct sx126x_hal_s *hal, uint8_t opcode, uint8_t *buffer, size_t len);
  sx126x_status_t (*wait_busy)(struct sx126x_hal_s *hal);
};

#ifdef __cplusplus
}
#endif

#endif // SX126X_HAL_H