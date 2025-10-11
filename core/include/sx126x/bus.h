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

// Forward declaration
typedef struct sx126x_bus_t sx126x_bus_t;

/**
 * @brief Represents a message bus for the SX126x.
 */
struct sx126x_bus_t
{
  sx126x_status_t (*transfer)(
      sx126x_bus_t *bus, const uint8_t *tx, size_t tx_len, uint8_t *rx, size_t rx_len);
  void (*log)(const char *fmt, ...);
  void *ctx;
};

#endif // SX126X_BUS_H
