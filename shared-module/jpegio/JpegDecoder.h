/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Jeff Epler for Adafruit Industries
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

#include "py/obj.h"
#include "lib/tjpgd/src/tjpgd.h"
#include "shared-module/displayio/Bitmap.h"

#define TJPGD_WORKSPACE_SIZE 3500

typedef struct jpegio_jpegdecoder_obj {
    mp_obj_base_t base;
    JDEC decoder;
    byte workspace[TJPGD_WORKSPACE_SIZE];
    mp_obj_t data_obj;
    mp_buffer_info_t bufinfo;
    displayio_bitmap_t *dest;
    uint16_t x, y;
    bitmaptools_rect_t lim;
    uint32_t skip_source_index, skip_dest_index;
    bool skip_source_index_none, skip_dest_index_none;
    uint8_t scale;
} jpegio_jpegdecoder_obj_t;
