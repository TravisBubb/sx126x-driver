// SPDX-License-Identifier: MIT

#include "freertos/FreeRTOS.h"
#include "sx126x/bus.h"
#include "sx126x/config.h"
#include "sx126x/hal_esp32.h"
#include "sx126x/sx126x.h"
#include <driver/spi_master.h>
#include <esp_log.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
  spi_transaction_t tran;
  uint8_t *owned_tx;
  uint8_t *owned_rx;
  bool free_tx;
  bool free_rx;
} esp32_spi_command_t;

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

static void lora_spi_task(void *arg)
{
  sx126x_hal_esp32_t *hal = arg;
  if (!hal)
  {
    return;
  }

  hal->bus.log("Starting SPI task...");

  while (!hal->is_shutdown_requested)
  {
    spi_transaction_t *completed;
    if (spi_device_get_trans_result(hal->lora_handle, &completed, pdMS_TO_TICKS(100)) == ESP_OK &&
        completed)
    {
      esp32_spi_command_t *cmd = (esp32_spi_command_t *)completed->user;
      if (cmd)
      {
        hal->bus.log("SPI result len %d", completed->length / 8);

        if (cmd->free_tx && cmd->owned_tx)
          heap_caps_free(cmd->owned_tx);

        if (cmd->free_rx && cmd->owned_rx)
          heap_caps_free(cmd->owned_rx);

        heap_caps_free(cmd);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1)); // short delay to yield CPU
  }

  // once shutdown is requested, drain any remaining transactions before exit
  spi_transaction_t *completed;
  while (spi_device_get_trans_result(hal->lora_handle, &completed, pdMS_TO_TICKS(10)) == ESP_OK)
  {
    esp32_spi_command_t *cmd = (esp32_spi_command_t *)completed->user;
    if (cmd)
    {
      if (cmd->free_tx && cmd->owned_tx)
        heap_caps_free(cmd->owned_tx);
      if (cmd->free_rx && cmd->owned_rx)
        heap_caps_free(cmd->owned_rx);
      heap_caps_free(cmd);
    }
  }

  hal->is_running = false;
  vTaskDelete(NULL);

  hal->bus.log("SPI task terminated.");
}

sx126x_status_t sx126x_hal_esp32_init(sx126x_hal_t *hal, const sx126x_hal_esp32_cfg_t *cfg)
{
  memset(hal, 0, sizeof(*hal));

  hal->bus.write = esp32_spi_write;
  hal->bus.read = esp32_spi_read;
  hal->bus.log = esp32_log;

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

  hal->is_shutdown_requested = false;
  hal->is_running = true;

  if (xTaskCreate(
          lora_spi_task, "lora_spi_task", 2048, hal, tskIDLE_PRIORITY + 1, &hal->spi_task_handle) !=
      pdPASS)
  {
    hal->bus.log("Failed to create SPI task.");
    return SX126X_ERR_UNKNOWN;
  }

  hal->bus.log("SPI initialized successfully.");
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