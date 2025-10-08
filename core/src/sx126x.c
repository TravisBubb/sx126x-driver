// SPDX-License-Identifier: MIT

#include "sx126x/sx126x.h"
#include "sx126x/bus.h"
#include "sx126x/types.h"
#include <stddef.h>
#include <stdint.h>

#define SX126X_LOG(bus, fmt, ...)                                                                  \
  do                                                                                               \
  {                                                                                                \
    if ((bus) && (bus)->log)                                                                       \
      (bus)->log(fmt, ##__VA_ARGS__);                                                              \
  } while (0)

sx126x_status_t sx126x_set_mode(sx126x_t *radio, sx126x_mode_t mode)
{
  SX126X_LOG(radio->bus, "Setting mode %d", mode);
  uint8_t cmd[] = {0x01, (uint8_t)mode};
  return radio->bus->write(cmd, sizeof(cmd)) ? SX126X_OK : SX126X_ERR_UNKNOWN;
}

static const uint32_t SX126X_FREQ_XTAL_HZ = 32000000; // 32 MHz

// Opcodes for the SX126x-class chip.
typedef enum
{
  SX126X_OP_SET_STANDBY = 0x80,
  SX126X_OP_SET_RF_FREQUENCY = 0x86,
  SX126X_OP_SET_PACKET_TYPE = 0x8A,
} sx126x_opcode_t;

// Standby modes for the SX126x-class chip.
typedef enum
{
  SX126X_STBY_RC = 0x00,
  SX126X_STBY_XOSC = 0x01,
} sx126x_standby_mode_t;

// Packet types for the SX126x-class chip.
typedef enum
{
  SX126X_PACKET_TYPE_GFSK = 0x00,
  SX126X_PACKET_TYPE_LORA = 0x01,
  SX126X_PACKET_TYPE_LR_FHSS = 0x03,
} sx126x_packet_type_t;

// Initialize the given radio instance
sx126x_status_t sx126x_init(sx126x_t *radio, sx126x_bus_t *bus, sx126x_config_t *config)
{
  SX126X_LOG(bus, "Initializing SX126x driver...");

  if (!radio || !bus)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  if (radio->is_initialized)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  // TODO: Validate that all of the function pointers are set in the hal

  // TODO: Validate any config options in config struct

  radio->bus = bus;

  sx126x_status_t ret;
  uint8_t tx_buffer[8];

  // Set standby mode to STDBY_RC
  tx_buffer[0] = SX126X_OP_SET_STANDBY;
  tx_buffer[1] = SX126X_STBY_RC;
  ret = radio->bus->write(tx_buffer, 2);
  if (ret != SX126X_OK)
  {
    return ret;
  }

  radio->mode = SX126X_MODE_STDBY_RC;

  // Set packet type to LoRa
  tx_buffer[0] = SX126X_OP_SET_PACKET_TYPE;
  tx_buffer[1] = SX126X_PACKET_TYPE_LORA;
  ;
  ret = radio->bus->write(tx_buffer, 2);
  if (ret != SX126X_OK)
  {
    return ret;
  }

  // Set RF frequency
  uint32_t frequency = (uint32_t)(((uint64_t)config->frequency_hz << 25) / SX126X_FREQ_XTAL_HZ);
  tx_buffer[0] = SX126X_OP_SET_RF_FREQUENCY;
  tx_buffer[1] = (frequency >> 24) & 0xFF;
  tx_buffer[2] = (frequency >> 16) & 0xFF;
  tx_buffer[3] = (frequency >> 8) & 0xFF;
  tx_buffer[4] = frequency & 0xFF;
  ret = radio->bus->write(tx_buffer, 5);
  if (ret != SX126X_OK)
  {
    return ret;
  }

  // TODO - chip initialization
  // [X] 1. `SetStandby(...)`: go to STDBY_RC mode if not already there
  // [X] 2. `SetPacketType(...)`: select LoRa protocol instead of FSK
  // [X] 3. `SetRfFrequency(...)`: set the RF frequency
  // [ ] 4. `SetPaConfig(...)`: define Power Amplifier configuration
  // [ ] 5. `SetTxParams(...)`: define output power and ramping time
  // [ ] 6. `SetModulationParams(...)`: SF, BW, CR (LoRa)
  // [ ] 7. `SetPacketParams(...)`: preamble, header mode, CRC, payload length mode
  // [ ] 8. `SetDioIrqParams(...)`: map `TxDone`/`RxDone` to DIO pins
  // [ ] 9. `WriteReg(...)`: for SyncWord if a non-default is needed

  SX126X_LOG(bus, "SX126x driver has been initialized.");

  return SX126X_OK;
}

// Deinitialize the given radio instance
sx126x_status_t sx126x_deinit(sx126x_t *radio)
{
  if (!radio)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  // TODO

  return SX126X_OK;
}
