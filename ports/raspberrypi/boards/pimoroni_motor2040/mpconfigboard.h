// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Pimoroni Motor 2040"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_NEOPIXEL         (&pin_GPIO18)
#define MICROPY_HW_NEOPIXEL_COUNT   (1)

#define DEFAULT_UART_BUS_TX (&pin_GPIO16)
#define DEFAULT_UART_BUS_RX (&pin_GPIO17)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO21)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO20)
