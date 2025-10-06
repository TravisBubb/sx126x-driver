# sx126x-driver

A modular, platform-agnostic driver stack for SX126x-class LoRa radios.

> ⚠️ Work in Progress — this repository is an active development effort and is not yet stable. Expect rapid iteration, breaking changes, and incomplete functionality.

The sx126x driver is a modular, platform-agnostic driver stack for SX126x-class LoRa radios. It separates hardware abstraction (HAL) from radio logic (Core), allowing flexible integration across embedded platforms such as the ESP32 or custom boards.

## Proposed Repository Structure

```
sx126x-driver/
├── core/                # Hardware-independent radio logic (SX126x ops, packet handling)
│   ├── include/         # Public headers for the core driver API
│   └── src/             # Core driver implementation
│
├── hal/                 # Platform-specific hardware abstraction layers
│   ├── esp32/           # Example HAL for ESP32 using ESP-IDF
│   └── mock/            # Mock HAL for unit testing
│
├── examples/            # Example applications using the driver
│   └── esp32_txrx/      # Simple transmit/receive demo (in progress)
│
├── docs/                # Architecture docs, design notes, diagrams
│
├── tests/               # Unit and integration tests (planned)
│
├── CMakeLists.txt       # Build configuration
├── LICENSE
└── README.md

```

## Design Goals

- **Platform Agnostic** - the core driver depends only on a thin HAL interface.
- **Modular** - easily swap or extend HALs for new MCU platforms.
- **RTOS Friendly** - integrate cleanly with FreeRTOS, using semaphores for concurrency.
- **Transparent Separation** - HAL manages MCU details; Core handles LoRa logic and registers.

## Contributing

Contributions are welcome, even while this is still in early development.

If you'd like to help:
1. Fork the repo
2. Create a branch for your feature or fix
3. Submit a PR with clear commit messages
4. Follow the code style used in the `core` and `hal` directories
5. Keep HALs platform-specific and core logic platform-agnostic

Discussions, design feedback, and issue reports are encouraged.

## License

This project is licensed under the **MIT License**. See [LICENSE](./LICENSE) for details.

## Tags

LoRa · SX1262 · SX126x · ESP32 · Embedded · IoT · HAL · FreeRTOS · C
