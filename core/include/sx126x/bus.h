// SPDX-License-Identifier: MIT

/**
 * @file bus.h
 * @brief Public API for the SX126x bus interface.
 * @version 0.1
 * @date 2025
 */

#ifndef SX126X_BUS_H
#define SX126X_BUS_H

#include "sx126x/types.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Represents a message bus for the SX126x.
 */
typedef struct
{
  sx126x_status_t (*write)(const uint8_t *data, size_t len);
  sx126x_status_t (*read)(uint8_t *data, size_t len);
  void (*log)(const char *fmt, ...);
} sx126x_bus_t;

#endif // SX126X_BUS_H
