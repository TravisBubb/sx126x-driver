<!-- SPDX-License-Identifier: MIT -->

# Driver Architecture

```
              ┌───────────────────────────┐
              │      User Application     │
              │ (main.c, examples, etc.)  │
              └────────────┬──────────────┘
                           │
                           │ includes
                           ▼
             ┌───────────────────────────────────┐
             │     Core Driver (sx126x)          │
             |  - Operates on sx126x_t           |
             │  - Initialized with sx126x_hal_t  │
             │  - Handles LoRa registers         │
             │  - Mode switching, TX/RX          │ 
             └────────────┬──────────────────────┘
                          │ depends on
                          ▼
          ┌─────────────────────────────────┐
          │     Public HAL Interface        │
          │     (sx126x_hal.h)              │
          │  - Defines sx126x_hal_t struct  │
          │  - Function pointers for SPI,   │
          │    GPIO, delays, etc.           │
          └────────────┬────────────────────┘
                       │ implemented by
                       ▼
        ┌───────────────────────────────────────┐
        │ Platform-specific HAL (HAL)           │
        │ hal/esp32/include/sx126x/hal_esp32.h  │
        │  - Implements SPI/GPIO funcs          │
        │  - Provides init:                     │
        │    sx126x_hal_esp32_init()            │
        │  - Configurable with board            │
        │    pin struct                         │
        └────────────┬──────────────────────────┘
                     │ initialized with
                     ▼
          ┌─────────────────────────┐
          │ Board-specific config   │
          │  - Defines which GPIOs  │
          │    are connected to     │
          │    DIO pins, CS, BUSY   │
          └─────────────────────────┘
```

## Usage Flow

1. User includes Core and HAL headers.
2. User initializes the platform-specific HAL with a board configuration struct.
3. Core driver receives HAL struct(`sx126x_hal_t`) and knows only the interface, not the platform.
4. Core driver executes LoRa operations using HAL function pointers (SPI read/write, GPIO read/write, delays, etc.).
5. Multiple HAL implementations (ESP32, STM32, Mock) can exist without changing Core logic.
6. Board-specific details (pin numbers, timing configs) are never hardcoded in Core. They live in the board config struct.