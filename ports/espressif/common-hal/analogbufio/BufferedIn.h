/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2023 Milind Movasha
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

#ifndef MICROPY_INCLUDED_ESP32_COMMON_HAL_ANALOGBUFIO_BUFFEREDIN_H
#define MICROPY_INCLUDED_ESP32_COMMON_HAL_ANALOGBUFIO_BUFFEREDIN_H

#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

#include "components/esp_adc/include/esp_adc/adc_continuous.h"

//  This is the analogbufio object
typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    uint32_t sample_rate;
    adc_continuous_handle_t handle;
    bool started;
} analogbufio_bufferedin_obj_t;

#endif // MICROPY_INCLUDED_ESP32_COMMON_HAL_ANALOGBUFIO_BUFFEREDIN_H
