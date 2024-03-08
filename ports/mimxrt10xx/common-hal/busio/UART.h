/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
 * Copyright (c) 2019 Artur Pacholec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/ringbuf.h"
#include "py/obj.h"
#include "periph.h"

#include "sdk/drivers/lpuart/fsl_lpuart.h"

typedef struct {
    mp_obj_base_t base;
    LPUART_Type *uart;
    lpuart_handle_t handle;
    uint8_t *ringbuf;
    uint32_t baudrate;
    uint32_t timeout_ms;
    uint8_t character_bits;
    uint8_t index;
    const mcu_pin_obj_t *rx;
    const mcu_pin_obj_t *tx;
    const mcu_pin_obj_t *cts;
    const mcu_pin_obj_t *rts;
    const mcu_pin_obj_t *rs485_dir;
    bool rs485_invert;

} busio_uart_obj_t;
