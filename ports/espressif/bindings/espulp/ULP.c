/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 microDev
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

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "bindings/espulp/ULP.h"

#include "py/enum.h"
#include "py/runtime.h"
#include "py/objproperty.h"

//| class ULP:
//|     def __init__(self, arch: Architecture = Architecture.FSM):
//|         """The ultra-low-power processor.
//|
//|         Raises an exception if another ULP has been instantiated. This
//|         ensures that is is only used by one piece of code at a time.
//|
//|         :param Architecture arch: The ulp arch"""
//|         ...
STATIC mp_obj_t espulp_ulp_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_arch };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_arch, MP_ARG_OBJ, {.u_obj = (void *)&architecture_FSM_obj} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const espulp_architecture_t arch = cp_enum_value(&espulp_architecture_type, args[ARG_arch].u_obj, MP_QSTR_arch);

    espulp_ulp_obj_t *self = mp_obj_malloc(espulp_ulp_obj_t, &espulp_ulp_type);

    common_hal_espulp_ulp_construct(self, arch);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void check_for_deinit(espulp_ulp_obj_t *self) {
    if (common_hal_espulp_ulp_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def deinit(self) -> None:
//|         """Deinitialises the ULP and releases it for another program."""
//|         ...
STATIC mp_obj_t espulp_ulp_deinit(mp_obj_t self_in) {
    espulp_ulp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_espulp_ulp_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espulp_ulp_deinit_obj, espulp_ulp_deinit);

//|     def __enter__(self) -> ULP:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
STATIC mp_obj_t espulp_ulp_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return espulp_ulp_deinit(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espulp_ulp___exit___obj, 4, 4, espulp_ulp_obj___exit__);

//|     def run(
//|         self, program: ReadableBuffer, *, pins: Sequence[microcontroller.Pin] = ()
//|     ) -> None:
//|         """Loads the program into ULP memory and then runs the program. The given pins are
//|            claimed and not reset until `halt()` is called.
//|
//|         The program will continue to run even when the running Python is halted."""
//|         ...
STATIC mp_obj_t espulp_ulp_run(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    espulp_ulp_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);

    enum { ARG_program, ARG_pins };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_program, MP_ARG_REQUIRED | MP_ARG_OBJ},
        { MP_QSTR_pins, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_empty_tuple} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_program].u_obj, &bufinfo, MP_BUFFER_READ);

    mp_obj_t pins_in = args[ARG_pins].u_obj;
    const size_t num_pins = (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(pins_in));

    // The ULP only supports 21 pins on the ESP32-S2 and S3. So we can store it
    // as a bitmask in a 32 bit number. The common-hal code does further checks.
    uint32_t pin_mask = 0;

    for (mp_uint_t i = 0; i < num_pins; i++) {
        mp_obj_t pin_obj = mp_obj_subscr(pins_in, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL);
        validate_obj_is_free_pin(pin_obj, MP_QSTR_pin);
        const mcu_pin_obj_t *pin = ((const mcu_pin_obj_t *)pin_obj);
        if (pin->number >= 32) {
            raise_ValueError_invalid_pin();
        }
        pin_mask |= 1 << pin->number;
    }

    common_hal_espulp_ulp_run(self, bufinfo.buf, bufinfo.len, pin_mask);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espulp_ulp_run_obj, 2, espulp_ulp_run);

//|     def halt(self) -> None:
//|         """Halts the running program and releases the pins given in `run()`."""
//|         ...
STATIC mp_obj_t espulp_ulp_halt(mp_obj_t self_in) {
    espulp_ulp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_espulp_ulp_halt(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espulp_ulp_halt_obj, espulp_ulp_halt);

//|     arch: Architecture
//|     """The ulp architecture. (read-only)"""
//|
STATIC mp_obj_t espulp_ulp_get_arch(mp_obj_t self_in) {
    espulp_ulp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return cp_enum_find(&espulp_architecture_type, self->arch);
}
MP_DEFINE_CONST_FUN_OBJ_1(espulp_ulp_get_arch_obj, espulp_ulp_get_arch);

MP_PROPERTY_GETTER(espulp_ulp_arch_obj,
    (mp_obj_t)&espulp_ulp_get_arch_obj);

STATIC const mp_rom_map_elem_t espulp_ulp_locals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),      MP_ROM_PTR(&espulp_ulp_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),   MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),    MP_ROM_PTR(&espulp_ulp___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_run),         MP_ROM_PTR(&espulp_ulp_run_obj) },
    { MP_ROM_QSTR(MP_QSTR_halt),        MP_ROM_PTR(&espulp_ulp_halt_obj) },
    { MP_ROM_QSTR(MP_QSTR_arch),        MP_ROM_PTR(&espulp_ulp_arch_obj) },
};
STATIC MP_DEFINE_CONST_DICT(espulp_ulp_locals_dict, espulp_ulp_locals_table);

MP_DEFINE_CONST_OBJ_TYPE(
    espulp_ulp_type,
    MP_QSTR_ULP,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, espulp_ulp_make_new,
    locals_dict, &espulp_ulp_locals_dict
    );
