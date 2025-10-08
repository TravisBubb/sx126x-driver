// SPDX-License-Identifier: MIT

/**
 * @file sx126x.h
 * @brief Public API for SX126x LoRa transceiver driver.
 * @version 0.1
 * @date 2025
 */

#ifndef SX126X_H
#define SX126X_H

#include "sx126x/bus.h"
#include "sx126x/types.h"

/**
 * @brief Radio operating modes.
 */
typedef enum
{
  SX126X_MODE_SLEEP = 0,
  SX126X_MODE_STDBY_RC,
  SX126X_MODE_STDBY_XOSC,
  SX126X_MODE_TX,
  SX126X_MODE_RX,
} sx126x_mode_t;

/**
 * @brief Represents configuration options for the SX126x.
 */
typedef struct
{
  uint32_t frequency_hz;
} sx126x_config_t;

/**
 * @brief Represents a LoRa radio instance.
 */
typedef struct
{
  bool is_initialized;
  sx126x_bus_t *bus;
  sx126x_mode_t mode;
} sx126x_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initialize the SX126x driver.
 *
 * The caller of this function must ensure that the lifetime of the hal pointer is valid for the
 * full lifetime of the sx126x_t struct.
 *
 * @param radio Pointer to the sx126x_t to initialize.
 * @param bus Pointer to the sx126x_bus_t to initialize the radio with.
 * @return SX126X_OK if successful, error code otherwise.
 */
sx126x_status_t sx126x_init(sx126x_t *radio, sx126x_bus_t *bus, sx126x_config_t *config);

/**
 * @brief Deinitialize the SX126x driver.
 * @param radio Pointer to the sx126x_t to deinitialize.
 * @return SX126X_OK if successful, error code otherwise.
 */
sx126x_status_t sx126x_deinit(sx126x_t *radio);

#ifdef __cplusplus
}
#endif

#endif // SX126X_H