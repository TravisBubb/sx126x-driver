// SPDX-License-Identifier: MIT

#include "freertos/FreeRTOS.h"
#include "sx126x/bus.h"
#include "sx126x/config.h"
#include "sx126x/hal_esp32.h"
#include "sx126x/sx126x.h"
#include <driver/spi_master.h>
#include <esp_log.h>
#include <string.h>

static sx126x_status_t esp32_spi_write(const uint8_t *data, size_t len)
{
  // TODO
  return ESP_OK;
}

static sx126x_status_t esp32_spi_read(uint8_t *data, size_t len)
{
  // TODO
  return ESP_OK;
}

static void esp32_log(const char *fmt, ...)
{
  char buf[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  ESP_LOGI("ESP32", "%s", buf);
}

sx126x_status_t sx126x_hal_esp32_init(sx126x_hal_t *hal, const sx126x_hal_esp32_cfg_t *cfg)
{
  memset(hal, 0, sizeof(*hal));

  hal->bus.write = esp32_spi_write;
  hal->bus.read = esp32_spi_read;
  hal->bus.log = esp32_log;

  // TODO - finish initialization

  return SX126X_OK;
}

sx126x_status_t sx126x_hal_esp32_deinit(sx126x_hal_t *hal)
{
  // TODO
  return SX126X_OK;
}

sx126x_bus_t *sx126x_hal_get_bus(sx126x_hal_t *hal)
{
  return &hal->bus;
}

sx126x_t *sx126x_hal_get_device(sx126x_hal_t *hal)
{
  return &hal->dev;
}