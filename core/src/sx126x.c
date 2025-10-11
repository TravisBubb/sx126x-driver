// SPDX-License-Identifier: MIT

#include "sx126x/sx126x.h"
#include "sx126x/bus.h"
#include "sx126x/types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define SX126X_LOG(bus, fmt, ...)                                                                  \
  do                                                                                               \
  {                                                                                                \
    if ((bus) && (bus)->log)                                                                       \
      (bus)->log(fmt, ##__VA_ARGS__);                                                              \
  } while (0)

static const uint32_t SX126X_FREQ_XTAL_HZ = 32000000; // 32 MHz

// Opcodes for the SX126x-class chip.
typedef enum
{
  SX126X_OP_READ_REGISTER = 0x1D,
  SX126X_OP_SET_STANDBY = 0x80,
  SX126X_OP_SET_RF_FREQUENCY = 0x86,
  SX126X_OP_SET_PACKET_TYPE = 0x8A,
  SX126X_OP_SET_MODULATION_PARAMS = 0x8B,
  SX126X_OP_SET_TX_PARAMS = 0x8E,
  SX126X_OP_SET_PA_CONFIG = 0x95,
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

// Configuratin values for PA.
typedef struct
{
  uint8_t pa_duty_cycle;
  uint8_t hp_max;
  sx126x_chip_variant_t chip;
  uint8_t pa_lut;
} sx126x_pa_config_t;

static sx126x_status_t sx126x_set_standby(sx126x_t *dev, sx126x_standby_mode_t mode);
static sx126x_status_t sx126x_set_packet_type(sx126x_t *dev, sx126x_modem_t modem);
static sx126x_status_t sx126x_set_frequency(sx126x_t *dev, uint32_t hz);
static sx126x_status_t sx126x_set_pa_profile(sx126x_t *dev, sx126x_pa_profile_t profile);
static sx126x_status_t
sx126x_set_tx_params(sx126x_t *dev, int pwr, sx126x_power_ramp_time_t ramp_time);
static sx126x_status_t sx126x_set_lora_modulation_params(sx126x_t *dev,
                                                         sx126x_lora_spreading_factor_t sf,
                                                         sx126x_lora_bandwidth_t bw,
                                                         sx126x_lora_coding_rate_t cr,
                                                         bool ldro);

static sx126x_status_t
sx126x_get_pa_configuration(sx126x_t *dev, sx126x_pa_profile_t profile, sx126x_pa_config_t *cfg);

// Initialize the given radio instance
sx126x_status_t sx126x_init(sx126x_t *dev, sx126x_bus_t *bus, sx126x_config_t *cfg)
{
  SX126X_LOG(bus, "Initializing SX126x driver...");

  if (!dev || !bus || !cfg)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  if (dev->is_initialized)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  if (!bus->transfer || !bus->log)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  memset(dev, 0, sizeof(*dev));
  dev->bus = bus;
  dev->chip = cfg->chip;

  sx126x_status_t st;

  SX126X_LOG(bus, "Setting RC standby mode...");
  st = sx126x_set_standby(dev, SX126X_STBY_RC);
  if (st != SX126X_OK)
  {
    SX126X_LOG(bus, "Failed to set RC standby mode.");
    return st;
  }
  SX126X_LOG(bus, "RC standby mode has been set.");

  SX126X_LOG(bus, "Setting packet type...");
  st = sx126x_set_packet_type(dev, cfg->modem);
  if (st != SX126X_OK)
  {
    SX126X_LOG(bus, "Failed to set packet type.");
    return st;
  }
  SX126X_LOG(bus, "Packet type has been set using modem %d.", cfg->modem);

  SX126X_LOG(bus, "Setting RF frequency...");
  st = sx126x_set_frequency(dev, cfg->frequency_hz);
  if (st != SX126X_OK)
  {
    SX126X_LOG(bus, "Failed to set RF frequency.");
    return st;
  }
  SX126X_LOG(bus, "RF frequency set to %dhz.", cfg->frequency_hz);

  SX126X_LOG(bus, "Setting PA profile...");
  st = sx126x_set_pa_profile(dev, cfg->pa_profile);
  if (st != SX126X_OK)
  {
    SX126X_LOG(bus, "Failed to set PA profile.");
    return st;
  }
  SX126X_LOG(bus, "PA profile %d has been set.", cfg->pa_profile);

  SX126X_LOG(bus, "Setting TX params...");
  st = sx126x_set_tx_params(dev, cfg->power_dbm, cfg->power_ramp_time);
  if (st != SX126X_OK)
  {
    SX126X_LOG(bus, "Failed to set TX params.");
    return st;
  }
  SX126X_LOG(bus, "TX params set.");

  if (cfg->modem == SX126X_MODEM_LORA)
  {
    SX126X_LOG(bus, "Setting LoRa modulation params...");
    st = sx126x_set_lora_modulation_params(
        dev, cfg->lora_sf, cfg->lora_bw, cfg->lora_cr, cfg->lora_ldro);
    if (st != SX126X_OK)
    {
      SX126X_LOG(bus, "Failed to set LoRa modulation params.");
      return st;
    }
    SX126X_LOG(bus, "LoRa modulation params set.");
  }
  else
  {
    // !NOTE: Only the LoRa modem is supported at this time.
    SX126X_LOG(bus, "Error: Only the LoRa modem is supported at this time.");
    return SX126X_ERR_INVALID_ARG;
  }

  // TODO - chip initialization
  // [X] 1. `SetStandby(...)`: go to STDBY_RC mode if not already there
  // [X] 2. `SetPacketType(...)`: select LoRa protocol instead of FSK
  // [X] 3. `SetRfFrequency(...)`: set the RF frequency
  // [X] 4. `SetPaConfig(...)`: define Power Amplifier configuration
  // [X] 5. `SetTxParams(...)`: define output power and ramping time
  // [X] 6. `SetModulationParams(...)`: SF, BW, CR (LoRa)
  // [ ] 7. `SetPacketParams(...)`: preamble, header mode, CRC, payload length mode
  // [ ] 8. `SetDioIrqParams(...)`: map `TxDone`/`RxDone` to DIO pins
  // [ ] 9. `WriteReg(...)`: for SyncWord if a non-default is needed

  dev->is_initialized = true;

  SX126X_LOG(bus,
             "SX126x init complete: chip=%d, freq=%lu Hz, PA=%d, pwr=%ddBm, pwr_ramp_time=%d, "
             "sf=%d, bw=%d, cr=%d, ldro=%d.",
             dev->chip,
             cfg->frequency_hz,
             cfg->pa_profile,
             cfg->power_dbm,
             cfg->power_ramp_time,
             cfg->lora_sf,
             cfg->lora_bw,
             cfg->lora_cr,
             cfg->lora_ldro);

  return SX126X_OK;
}

// Deinitialize the given radio instance
sx126x_status_t sx126x_deinit(sx126x_t *dev)
{
  if (!dev)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  if (dev->is_initialized)
  {
    // Attempt to put the chip in RC standby (lowest power safe state)
    if (dev->bus)
    {
      sx126x_status_t st = sx126x_set_standby(dev, SX126X_STBY_RC);
      if (st != SX126X_OK)
      {
        SX126X_LOG(dev->bus, "Warning: Failed to set standby during deinit.");
      }
    }

    memset(dev, 0, sizeof(*dev));
    dev->is_initialized = false;
    dev->bus = NULL;
  }

  return SX126X_OK;
}

static sx126x_status_t sx126x_set_standby(sx126x_t *dev, sx126x_standby_mode_t mode)
{
  if (!dev || !dev->bus || !dev->bus->transfer)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  uint8_t tx[] = {SX126X_OP_SET_STANDBY, mode};
  return dev->bus->transfer(dev->bus, tx, sizeof(tx), NULL, 0);
}

static sx126x_status_t sx126x_set_packet_type(sx126x_t *dev, sx126x_modem_t modem)
{
  if (!dev || !dev->bus || !dev->bus->transfer)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  sx126x_packet_type_t pkt_type;
  if (modem == SX126X_MODEM_LORA)
    pkt_type = SX126X_PACKET_TYPE_LORA;
  else
    pkt_type = SX126X_PACKET_TYPE_GFSK;

  uint8_t tx[] = {SX126X_OP_SET_PACKET_TYPE, pkt_type};
  return dev->bus->transfer(dev->bus, tx, sizeof(tx), NULL, 0);
}

static sx126x_status_t sx126x_set_frequency(sx126x_t *dev, uint32_t hz)
{
  if (!dev || !dev->bus || !dev->bus->transfer)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  uint32_t frequency = (uint32_t)(((uint64_t)hz << 25) / SX126X_FREQ_XTAL_HZ);
  uint8_t tx[] = {
      SX126X_OP_SET_RF_FREQUENCY,
      (frequency >> 24) & 0xFF,
      (frequency >> 16) & 0xFF,
      (frequency >> 8) & 0xFF,
      frequency & 0xFF,
  };
  return dev->bus->transfer(dev->bus, tx, sizeof(tx), NULL, 0);
}

static sx126x_status_t sx126x_set_pa_profile(sx126x_t *dev, sx126x_pa_profile_t profile)
{
  if (!dev || !dev->bus || !dev->bus->transfer)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  sx126x_pa_config_t cfg;
  sx126x_status_t st;

  st = sx126x_get_pa_configuration(dev, profile, &cfg);
  if (st != SX126X_OK)
  {
    return st;
  }

  uint8_t tx[] = {
      SX126X_OP_SET_PA_CONFIG,
      cfg.pa_duty_cycle,
      cfg.hp_max,
      cfg.chip,
      cfg.pa_lut,
  };

  st = dev->bus->transfer(dev->bus, tx, sizeof(tx), NULL, 0);
  if (st == SX126X_OK)
    dev->pa_profile = profile;

  return st;
}

static sx126x_status_t
sx126x_get_pa_configuration(sx126x_t *dev, sx126x_pa_profile_t profile, sx126x_pa_config_t *cfg)
{
  if (!dev)
    return SX126X_ERR_INVALID_ARG;

  switch (dev->chip)
  {
  case SX126X_CHIP_SX1261:
    // SX1261 supports only low power (14 dBm)
    cfg->pa_duty_cycle = 0x04;
    cfg->hp_max = 0x00;
    cfg->chip = 0x01;
    cfg->pa_lut = 0x01;

    // Force low power regardless of user selection
    if (profile != SX126X_PA_LOW_POWER && dev->bus && dev->bus->log)
      dev->bus->log("Warning: SX1261 only supports LOW_POWER. Overriding user profile.");
    break;

  case SX126X_CHIP_SX1262:
    switch (profile)
    {
    case SX126X_PA_LOW_POWER:
      // ~14 dBm configuration
      cfg->pa_duty_cycle = 0x04;
      cfg->hp_max = 0x00;
      cfg->chip = 0x00;
      cfg->pa_lut = 0x01;
      break;

    case SX126X_PA_MEDIUM_POWER:
      // ~17 dBm configuration
      cfg->pa_duty_cycle = 0x06;
      cfg->hp_max = 0x03;
      cfg->chip = 0x00;
      cfg->pa_lut = 0x01;
      break;

    case SX126X_PA_HIGH_POWER:
      // +20 dBm configuration (safe max for SX1262)
      cfg->pa_duty_cycle = 0x07;
      cfg->hp_max = 0x05;
      cfg->chip = 0x00;
      cfg->pa_lut = 0x01;
      break;
    }
    break;

  default:
    // Unknown chip: safest possible configuration
    cfg->pa_duty_cycle = 0x04;
    cfg->hp_max = 0x00;
    cfg->chip = 0x00;
    cfg->pa_lut = 0x01;
    if (dev->bus && dev->bus->log)
      dev->bus->log("Unknown SX126x chip — using default low power PA config.");
    break;
  }

  return SX126X_OK;
}

static sx126x_status_t
sx126x_set_tx_params(sx126x_t *dev, int pwr, sx126x_power_ramp_time_t ramp_time)
{
  if (!dev || !dev->bus || !dev->bus->transfer)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  uint8_t tx[] = {
      SX126X_OP_SET_TX_PARAMS,
      pwr,
      ramp_time,
  };

  return dev->bus->transfer(dev->bus, tx, sizeof(tx), NULL, 0);
}

static sx126x_status_t sx126x_set_lora_modulation_params(sx126x_t *dev,
                                                         sx126x_lora_spreading_factor_t sf,
                                                         sx126x_lora_bandwidth_t bw,
                                                         sx126x_lora_coding_rate_t cr,
                                                         bool ldro)
{
  if (!dev || !dev->bus || !dev->bus->transfer)
  {
    return SX126X_ERR_INVALID_ARG;
  }

  uint8_t tx[] = {SX126X_OP_SET_MODULATION_PARAMS, sf, bw, cr, ldro};

  return dev->bus->transfer(dev->bus, tx, sizeof(tx), NULL, 0);
}