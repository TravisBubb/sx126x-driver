// SPDX-License-Identifier: MIT

/**
 * @file hal_esp32.h
 * @brief ESP32-specific HAL config and factory for the SX126x driver.
 * @version 0.1
 * @date 2025
 */

#ifndef SX126X_HAL_ESP32_H
#define SX126X_HAL_ESP32_H

#include "sx126x/hal.h"
#include "sx126x/sx126x.h"
#include <driver/spi_master.h>

/**
 * @brief Configuration options for the ESP32 HAL.
 */
typedef struct
{
  int spi_host;              /**< SPI host number */
  int spi_mosi_pin;          /**< GPIO pin number for MOSI */
  int spi_miso_pin;          /**< GPIO pin number for MISO */
  int spi_sclk_pin;          /**< GPIO pin for SCLK */
  int spi_max_transfer_size; /**< Maximum transfer size, 0 for default */
  int spi_cs_pin;            /**< GPIO pin number for SPI chip select */
  int spi_clock_speed_hz;    /**< SPI clock speed in Hz */
  int spi_queue_size;        /**< SPI queue size */
} sx126x_hal_esp32_cfg_t;

/**
 * @brief Represents an ESP32 HAL instance.
 */
typedef struct sx126x_hal_s
{
  int spi_host;
  sx126x_bus_t bus;
  sx126x_t dev;
  volatile bool is_running;
  volatile bool is_shutdown_requested;
  SemaphoreHandle_t spi_mutex;
  spi_device_handle_t lora_handle;
  TaskHandle_t spi_task_handle;
} sx126x_hal_esp32_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Convenience factory that fills *out with an sx126x_hal_t instance of the ESP32 HAL.
 */
sx126x_status_t sx126x_hal_esp32_init(sx126x_hal_t *out, const sx126x_hal_esp32_cfg_t *cfg);

/**
 * @brief Convenience destory wrapper around sx126x_hal_deinit.
 */
sx126x_status_t sx126x_hal_esp32_deinit(sx126x_hal_t *hal);

#ifdef __cplusplus
}
#endif

#endif // SX126X_HAL_ESP32_H