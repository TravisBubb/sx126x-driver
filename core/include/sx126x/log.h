// SPDX-License-Identifier: MIT

/**
 * @file log.h
 * @brief Internal SX126x driver logging macros.
 * @version 0.1
 * @date 2025
 */

#ifndef SX126X_LOG_H
#define SX126X_LOG_H

// Log levels
#define SX126X_LOG_LEVEL_ERROR 0
#define SX126X_LOG_LEVEL_WARN 1
#define SX126X_LOG_LEVEL_INFO 2
#define SX126X_LOG_LEVEL_DEBUG 3

// Default log level (can be overidden via a compiler flag)
#ifndef SX126X_LOG_LEVEL
#define SX126X_LOG_LEVEL SX126X_LOG_LEVEL_INFO
#endif

// Base macro - only emits code if level <= build level
#define SX126X_LOG_INTERNAL(level, bus, tag, fmt, ...)                                             \
  do                                                                                               \
  {                                                                                                \
    if ((level) <= SX126X_LOG_LEVEL)                                                               \
    {                                                                                              \
      if ((bus) && (bus)->log)                                                                     \
        (bus)->log("[%s] " fmt, tag, ##__VA_ARGS__);                                              \
    }                                                                                              \
  } while (0)

// User-facing macros — these can be compiled out entirely
#if SX126X_LOG_LEVEL >= SX126X_LOG_LEVEL_ERROR
#define SX126X_LOG_ERROR(bus, fmt, ...)  SX126X_LOG_INTERNAL(SX126X_LOG_LEVEL_ERROR, bus, "ERROR", fmt, ##__VA_ARGS__)
#else
#define SX126X_LOG_ERROR(bus, fmt, ...)  ((void)0)
#endif

#if SX126X_LOG_LEVEL >= SX126X_LOG_LEVEL_WARN
#define SX126X_LOG_WARN(bus, fmt, ...)   SX126X_LOG_INTERNAL(SX126X_LOG_LEVEL_WARN, bus, "WARN", fmt, ##__VA_ARGS__)
#else
#define SX126X_LOG_WARN(bus, fmt, ...)   ((void)0)
#endif

#if SX126X_LOG_LEVEL >= SX126X_LOG_LEVEL_INFO
#define SX126X_LOG_INFO(bus, fmt, ...)   SX126X_LOG_INTERNAL(SX126X_LOG_LEVEL_INFO, bus, "INFO", fmt, ##__VA_ARGS__)
#else
#define SX126X_LOG_INFO(bus, fmt, ...)   ((void)0)
#endif

#if SX126X_LOG_LEVEL >= SX126X_LOG_LEVEL_DEBUG
#define SX126X_LOG_DEBUG(bus, fmt, ...)  SX126X_LOG_INTERNAL(SX126X_LOG_LEVEL_DEBUG, bus, "DEBUG", fmt, ##__VA_ARGS__)
#else
#define SX126X_LOG_DEBUG(bus, fmt, ...)  ((void)0)
#endif

#endif // SX126X_LOG_H
