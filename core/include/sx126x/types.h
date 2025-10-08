// SPDX-License-Identifier: MIT

/**
 * @file sx126x.h
 * @brief Public API for SX126x LoRa transceiver driver.
 * @version 0.1
 * @date 2025
 */

#ifndef SX126X_TYPES_H
#define SX126X_TYPES_H

/**
 * @brief Status codes returned by the SX126X driver.
 */
typedef enum
{
  SX126X_OK = 0,
  SX126X_ERR_INVALID_ARG,
  SX126X_ERR_BUSY, /* driver or radio in a state that prevents request */
  SX126X_ERR_TIMEOUT,
  SX126X_ERR_NO_MEM,
  SX126X_ERR_HAL,      /* underlying HAL reported an error */
  SX126X_ERR_NOT_INIT, /* driver not initialized */
  SX126X_ERR_UNKNOWN,
} sx126x_status_t;

#endif // SX126X_TYPES_H