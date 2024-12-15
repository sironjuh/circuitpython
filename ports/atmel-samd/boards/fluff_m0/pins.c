// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_AREF), MP_ROM_PTR(&pin_PA03) },

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PA08) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_PA07) },

    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_PA31) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_SDO), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_PA30) },
    { MP_ROM_QSTR(MP_QSTR_SDI), MP_ROM_PTR(&pin_PA30) },

    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PA10) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_PA10) },

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PA22) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PA23) },

    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PA14) },
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PA18) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PA16) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_PA19) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PA17) },
    { MP_ROM_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_PA27) },
    { MP_ROM_QSTR(MP_QSTR_EN), MP_ROM_PTR(&pin_PA27) },
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_PA28) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);