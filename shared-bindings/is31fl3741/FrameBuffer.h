/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mark Komus
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

#include "shared-module/is31fl3741/FrameBuffer.h"
#include "shared-module/is31fl3741/IS31FL3741.h"

extern const mp_obj_type_t is31fl3741_framebuffer_type;

void common_hal_is31fl3741_framebuffer_construct(is31fl3741_framebuffer_obj_t *self, int width, int height, mp_obj_t framebuffer, is31fl3741_IS31FL3741_obj_t *is31, mp_obj_t mapping);

void common_hal_is31fl3741_framebuffer_deinit(is31fl3741_framebuffer_obj_t *);

int common_hal_is31fl3741_framebuffer_get_width(is31fl3741_framebuffer_obj_t *self);
int common_hal_is31fl3741_framebuffer_get_height(is31fl3741_framebuffer_obj_t *self);

void common_hal_is31fl3741_framebuffer_set_global_current(is31fl3741_framebuffer_obj_t *self, uint8_t current);
uint8_t common_hal_is31fl3741_framebuffer_get_global_current(is31fl3741_framebuffer_obj_t *self);

void common_hal_is31fl3741_framebuffer_set_paused(is31fl3741_framebuffer_obj_t *self, bool paused);
bool common_hal_is31fl3741_framebuffer_get_paused(is31fl3741_framebuffer_obj_t *self);
void common_hal_is31fl3741_framebuffer_refresh(is31fl3741_framebuffer_obj_t *self, uint8_t *dirtyrows);

void common_hal_is31fl3741_framebuffer_reconstruct(is31fl3741_framebuffer_obj_t *self, mp_obj_t framebuffer);

void is31fl3741_framebuffer_collect_ptrs(is31fl3741_framebuffer_obj_t *self);
