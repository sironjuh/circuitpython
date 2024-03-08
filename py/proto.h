/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jeff Epler for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_PY_PROTO_H
#define MICROPY_INCLUDED_PY_PROTO_H

// CIRCUITPY-CHANGE
//
// In CircuitPython, we have multiple different 'protocol' types (rather than multiplexing through ioctl).
// The standard (read/write/ioctl) protocol is indicated by the "0" protocol, while others must MP_PROTO_IMPLEMENT(QSTR_...).
// While not used in CircuitPython or tested, the ability to skip including the protocol field in a protocol object is included at the source level

#ifdef MICROPY_UNSAFE_PROTO
#define MP_PROTOCOL_HEAD /* NOTHING */
#define MP_PROTO_IMPLEMENT(name) /* NOTHING */
static inline void *mp_proto_get(uint16_t name, mp_const_obj_type_t obj) {
    return MP_OBJ_TYPE_GET_SLOT_OR_NULL(mp_obj_get_type(obj), protocol);
}
#else
#define MP_PROTOCOL_HEAD \
    uint16_t name;
#define MP_PROTO_IMPLEMENT(n) .name = n,
const void *mp_proto_get(uint16_t name, mp_const_obj_t obj);
const void *mp_proto_get_or_throw(uint16_t name, mp_const_obj_t obj);
#endif

#endif
