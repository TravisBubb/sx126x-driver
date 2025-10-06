// SPDX-License-Identifier: MIT

/**
 * @file sx126x.h
 * @brief Public API for SX126x LoRa transceiver driver.
 * @version 0.1
 * @date 2025
 */

#ifndef SX126X_H
#define SX126X_H

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

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initialize the SX126x driver.
 * @return SX126X_OK if successful, error code otherwise.
 */
sx126x_status_t sx126x_init(void);

/**
 * @brief Deinitialize the SX126x driver.
 * @return SX126X_OK if successful, error code otherwise.
 */
sx126x_status_t sx126x_deinit(void);

#ifdef __cplusplus
}
#endif

#endif // SX126X_H