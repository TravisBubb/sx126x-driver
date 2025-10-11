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
 * @brief SX126X state.
 */
typedef enum {
  SX126X_STATE_INIT = 0,
  SX126X_STATE_DEINIT,
  SX126X_STATE_STANDBY,
  SX126X_STATE_TX,
  SX126X_STATE_RX,
} sx126x_state_t;

/**
 * @brief SX126X chip variant.
 */
typedef enum
{
  SX126X_CHIP_SX1262 = 0x0,
  SX126X_CHIP_SX1261 = 0x1,
} sx126x_chip_variant_t;

/**
 * @brief Power profile modes.
 */
typedef enum
{
  SX126X_PA_LOW_POWER,    /**< ~14dBm */
  SX126X_PA_MEDIUM_POWER, /**< ~17dBm */
  SX126X_PA_HIGH_POWER,   /**< ~22 dBm */
} sx126x_pa_profile_t;

/**
 * @brief Modem type.
 */
typedef enum
{
  SX126X_MODEM_LORA,
  SX126X_MODEM_FSK,
} sx126x_modem_t;

/**
 * @brief Power ramp times.
 */
typedef enum
{
  SX126X_PWR_RAMP_TIME_10U = 0x00,   /**< 10us */
  SX126X_PWR_RAMP_TIME_20U = 0x01,   /**< 20us */
  SX126X_PWR_RAMP_TIME_40U = 0x02,   /**< 40us */
  SX126X_PWR_RAMP_TIME_80U = 0x03,   /**< 80us */
  SX126X_PWR_RAMP_TIME_200U = 0x04,  /**< 200us*/
  SX126X_PWR_RAMP_TIME_800U = 0x05,  /**< 800us */
  SX126X_PWR_RAMP_TIME_1700U = 0x06, /**< 1700us */
  SX126X_PWR_RAMP_TIME_3400U = 0x07, /**< 3400us */
} sx126x_power_ramp_time_t;

/**
 * @brief LoRa spreading factor.
 */
typedef enum
{
  SX126X_LORA_SF_5 = 0x05,
  SX126X_LORA_SF_6 = 0x06,
  SX126X_LORA_SF_7 = 0x07,
  SX126X_LORA_SF_8 = 0x08,
  SX126X_LORA_SF_9 = 0x09,
  SX126X_LORA_SF_10 = 0x0A,
  SX126X_LORA_SF_11 = 0x0B,
  SX126X_LORA_SF_12 = 0x0C,
} sx126x_lora_spreading_factor_t;

/**
 * @brief LoRa bandwidth.
 */
typedef enum
{
  SX126X_LORA_BW_7 = 0x00,   /**< 7.8kHz real */
  SX126X_LORA_BW_10 = 0x08,  /**< 10.42kHz real */
  SX126X_LORA_BW_15 = 0x01,  /**< 15.63kHz real */
  SX126X_LORA_BW_20 = 0x09,  /**< 20.83kHz real */
  SX126X_LORA_BW_32 = 0x02,  /**< 31.25kHz real */
  SX126X_LORA_BW_41 = 0x0A,  /**< 41.67kHz real */
  SX126X_LORA_BW_62 = 0x03,  /**< 62.50kHz real */
  SX126X_LORA_BW_125 = 0x04, /**< 125kHz real */
  SX126X_LORA_BW_250 = 0x05, /**< 250kHz real */
  SX126X_LORA_BW_500 = 0x06, /**< 500kHz real */
} sx126x_lora_bandwidth_t;

/**
 * @brief LoRa coding rate.
 */
typedef enum {
  SX126X_LORA_CR_4_5 = 0x01,
  SX126X_LORA_CR_4_6 = 0x02,
  SX126X_LORA_CR_4_7 = 0x03,
  SX126X_LORA_CR_4_8 = 0x04,
  SX126X_LORA_CR_4_5_LI = 0x05,
  SX126X_LORA_CR_4_6_LI = 0x06,
  SX126X_LORA_CR_4_8_LI = 0x07,
} sx126x_lora_coding_rate_t;

/**
 * @brief Represents configuration options for the SX126x.
 */
typedef struct
{
  sx126x_chip_variant_t chip;
  uint32_t frequency_hz;
  sx126x_pa_profile_t pa_profile;
  sx126x_modem_t modem;

  int power_dbm; /**< Output power in dBm. If low power PA is selected, then -17 to +14dBm. If high
                    power PA is selected, then -9 to +22dBm. */
  sx126x_power_ramp_time_t power_ramp_time;

  sx126x_lora_spreading_factor_t lora_sf;
  sx126x_lora_bandwidth_t lora_bw;
  sx126x_lora_coding_rate_t lora_cr;
  bool lora_ldro; /**< Enable/Disable Low Data Rate Optimization (LDRO). */
} sx126x_config_t;

/**
 * @brief Represents a LoRa radio instance.
 */
typedef struct
{
  bool is_initialized;
  sx126x_state_t state;
  sx126x_bus_t *bus;
  sx126x_chip_variant_t chip;
  sx126x_pa_profile_t pa_profile;
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