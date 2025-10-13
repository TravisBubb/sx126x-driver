// SPDX-License-Identifier: MIT

#include "freertos/FreeRTOS.h"
#include "sx126x/bus.h"
#include "sx126x/hal_esp32.h"
#include "sx126x/sx126x.h"
#include <driver/spi_master.h>
#include <esp_log.h>
#include <stdbool.h>
#include <string.h>

static sx126x_status_t esp32_spi_transfer(sx126x_bus_t *bus, const uint8_t *tx, size_t tx_len, uint8_t *rx, size_t rx_len)
{
  if (!bus || !(tx && !rx) || (tx_len == 0 && rx_len == 0))
  {
    return SX126X_ERR_INVALID_ARG;
  }

  sx126x_hal_esp32_t *hal = (sx126x_hal_esp32_t *)bus->ctx;
  if (!hal || !hal->lora_handle)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  size_t len = tx_len > rx_len ? tx_len : rx_len;

  uint8_t dummy_tx[len];
  const uint8_t *tx_buf = tx ? tx : dummy_tx;
  if (!tx)
  {
    memset(dummy_tx, 0x00, len);
  }

  uint8_t dummy_rx[len];
  uint8_t *rx_buf = rx ? rx : dummy_rx;

  spi_transaction_t t = {
    .length = len * 8,
    .tx_buffer = tx_buf,
    .rx_buffer = rx_buf,
  };

  xSemaphoreTake(hal->spi_mutex, portMAX_DELAY);
  esp_err_t ret = spi_device_transmit(hal->lora_handle, &t);
  xSemaphoreGive(hal->spi_mutex);

  return (ret == ESP_OK) ? SX126X_OK : SX126X_ERR_IO;
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

  hal->bus.transfer = esp32_spi_transfer;
  hal->bus.log = esp32_log;
  hal->bus.ctx = hal;

  hal->bus.log("Initializing SPI...");

  hal->spi_mutex = xSemaphoreCreateMutex();
  if (!hal->spi_mutex)
  {
    hal->bus.log("Failed to create SPI mutex.");
    return SX126X_ERR_NO_MEM;
  }

  spi_bus_config_t buscfg = {.mosi_io_num = cfg->spi_mosi_pin,
                             .miso_io_num = cfg->spi_miso_pin,
                             .sclk_io_num = cfg->spi_sclk_pin,
                             .quadwp_io_num = -1,
                             .quadhd_io_num = -1,
                             .max_transfer_sz = cfg->spi_max_transfer_size};

  esp_err_t ret = spi_bus_initialize(cfg->spi_host, &buscfg, SPI_DMA_CH_AUTO);
  if (ret != ESP_OK)
  {
    hal->bus.log("Failed to init SPI bus with status: %d.", ret);
    return SX126X_ERR_UNKNOWN;
  }

  spi_device_interface_config_t devcfg = {
      .clock_speed_hz = cfg->spi_clock_speed_hz,
      .mode = 0,
      .spics_io_num = cfg->spi_cs_pin,
      .queue_size = cfg->spi_queue_size,
  };

  ret = spi_bus_add_device(cfg->spi_host, &devcfg, &hal->lora_handle);
  if (ret != ESP_OK)
  {
    hal->bus.log("Failed to add SPI device with status: %d.", ret);
    return SX126X_ERR_UNKNOWN;
  }

  hal->spi_host = cfg->spi_host;
  hal->is_shutdown_requested = false;
  hal->is_running = true;
  hal->bus.log("SPI initialized successfully.");

  return SX126X_OK;
}

sx126x_status_t sx126x_hal_esp32_deinit(sx126x_hal_t *hal)
{
  if (!hal)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  hal->is_shutdown_requested = true;

  if (hal->lora_handle)
  {
    spi_bus_remove_device(hal->lora_handle);
    hal->lora_handle = NULL;
  }

  spi_bus_free(hal->spi_host);

  if (hal->spi_mutex)
  {
    vSemaphoreDelete(hal->spi_mutex);
    hal->spi_mutex = NULL;
  }

  memset(hal, 0, sizeof(*hal));

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