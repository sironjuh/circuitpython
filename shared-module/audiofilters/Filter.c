// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiofilters/Filter.h"

#include <stdint.h>
#include "py/runtime.h"

void common_hal_audiofilters_filter_construct(audiofilters_filter_obj_t *self,
    mp_obj_t filter, mp_obj_t mix,
    uint32_t buffer_size, uint8_t bits_per_sample,
    bool samples_signed, uint8_t channel_count, uint32_t sample_rate) {

    // Basic settings every effect and audio sample has
    // These are the effects values, not the source sample(s)
    self->bits_per_sample = bits_per_sample; // Most common is 16, but 8 is also supported in many places
    self->samples_signed = samples_signed; // Are the samples we provide signed (common is true)
    self->channel_count = channel_count; // Channels can be 1 for mono or 2 for stereo
    self->sample_rate = sample_rate; // Sample rate for the effect, this generally needs to match all audio objects

    // To smooth things out as CircuitPython is doing other tasks most audio objects have a buffer
    // A double buffer is set up here so the audio output can use DMA on buffer 1 while we
    // write to and create buffer 2.
    // This buffer is what is passed to the audio component that plays the effect.
    // Samples are set sequentially. For stereo audio they are passed L/R/L/R/...
    self->buffer_len = buffer_size; // in bytes

    self->buffer[0] = m_malloc(self->buffer_len);
    memset(self->buffer[0], 0, self->buffer_len);

    self->buffer[1] = m_malloc(self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);

    self->last_buf_idx = 1; // Which buffer to use first, toggle between 0 and 1

    // This buffer will be used to process samples through the biquad filter
    self->filter_buffer = m_malloc(SYNTHIO_MAX_DUR * sizeof(int32_t));
    memset(self->filter_buffer, 0, SYNTHIO_MAX_DUR * sizeof(int32_t));

    // Initialize other values most effects will need.
    self->sample = NULL; // The current playing sample
    self->sample_remaining_buffer = NULL; // Pointer to the start of the sample buffer we have not played
    self->sample_buffer_length = 0; // How many samples do we have left to play (these may be 16 bit!)
    self->loop = false; // When the sample is done do we loop to the start again or stop (e.g. in a wav file)
    self->more_data = false; // Is there still more data to read from the sample or did we finish

    // The below section sets up the effect's starting values.

    if (filter == MP_OBJ_NULL) {
        filter = mp_const_none;
    }
    common_hal_audiofilters_filter_set_filter(self, filter);

    // If we did not receive a BlockInput we need to create a default float value
    if (mix == MP_OBJ_NULL) {
        mix = mp_obj_new_float(1.0);
    }
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);
}

bool common_hal_audiofilters_filter_deinited(audiofilters_filter_obj_t *self) {
    if (self->buffer[0] == NULL) {
        return true;
    }
    return false;
}

void common_hal_audiofilters_filter_deinit(audiofilters_filter_obj_t *self) {
    if (common_hal_audiofilters_filter_deinited(self)) {
        return;
    }
    self->buffer[0] = NULL;
    self->buffer[1] = NULL;
    self->filter_buffer = NULL;
    self->filter_states = NULL;
}

void reset_filter_states(audiofilters_filter_obj_t *self) {
    self->filter_states_len = 0;
    self->filter_states = NULL;

    mp_obj_t *items;
    if (mp_obj_is_type(self->filter, (const mp_obj_type_t *)&synthio_biquad_type_obj)) {
        self->filter_states_len = 1;
        items = self->filter;
    } else if (mp_obj_is_tuple_compatible(self->filter)) {
        mp_obj_tuple_get(self->filter, &self->filter_states_len, &items);
    }

    if (!self->filter_states_len) {
        return;
    }

    self->filter_states = m_malloc(self->filter_states_len * sizeof(biquad_filter_state));

    if (mp_obj_is_type(items, (const mp_obj_type_t *)&synthio_biquad_type_obj)) {
        synthio_biquad_filter_assign(&self->filter_states[0], items);
    } else {
        for (size_t i = 0; i < self->filter_states_len; i++) {
            synthio_biquad_filter_assign(&self->filter_states[i], items[i]);
        }
    }
}

mp_obj_t common_hal_audiofilters_filter_get_filter(audiofilters_filter_obj_t *self) {
    if (mp_obj_is_type(self->filter, (const mp_obj_type_t *)&synthio_biquad_type_obj) || mp_obj_is_tuple_compatible(self->filter)) {
        return self->filter;
    } else {
        return mp_const_none;
    }
}

void common_hal_audiofilters_filter_set_filter(audiofilters_filter_obj_t *self, mp_obj_t arg) {
    if (arg == mp_const_none || mp_obj_is_type(arg, (const mp_obj_type_t *)&synthio_biquad_type_obj)) {
        self->filter = arg;
    } else if (mp_obj_is_tuple_compatible(arg) || mp_obj_is_type(arg, &mp_type_list)) {
        size_t tuple_len;
        mp_obj_t *tuple_items = NULL;

        mp_obj_get_array(arg, &tuple_len, &tuple_items);

        mp_obj_t *biquad_objects[tuple_len];
        for (size_t i = 0; i < tuple_len; i++) {
            biquad_objects[i] = mp_arg_validate_type_in(tuple_items[i], (const mp_obj_type_t *)&synthio_biquad_type_obj, MP_QSTR_filter);
        }

        self->filter = mp_obj_new_tuple(tuple_len, (const mp_obj_t *)biquad_objects);
    } else {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be a %q object, %q, or %q"), MP_QSTR_filter, MP_QSTR_Biquad, MP_QSTR_tuple, MP_QSTR_None);
    }

    reset_filter_states(self);
}

mp_obj_t common_hal_audiofilters_filter_get_mix(audiofilters_filter_obj_t *self) {
    return self->mix.obj;
}

void common_hal_audiofilters_filter_set_mix(audiofilters_filter_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->mix, MP_QSTR_mix);
}

uint32_t common_hal_audiofilters_filter_get_sample_rate(audiofilters_filter_obj_t *self) {
    return self->sample_rate;
}

uint8_t common_hal_audiofilters_filter_get_channel_count(audiofilters_filter_obj_t *self) {
    return self->channel_count;
}

uint8_t common_hal_audiofilters_filter_get_bits_per_sample(audiofilters_filter_obj_t *self) {
    return self->bits_per_sample;
}

void audiofilters_filter_reset_buffer(audiofilters_filter_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    memset(self->buffer[0], 0, self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);
    memset(self->filter_buffer, 0, SYNTHIO_MAX_DUR * sizeof(int32_t));

    if (self->filter_states) {
        for (uint8_t i = 0; i < self->filter_states_len; i++) {
            synthio_biquad_filter_reset(&self->filter_states[i]);
        }
    }
}

bool common_hal_audiofilters_filter_get_playing(audiofilters_filter_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiofilters_filter_play(audiofilters_filter_obj_t *self, mp_obj_t sample, bool loop) {
    // When a sample is to be played we must ensure the samples values matches what we expect
    // Then we reset the sample and get the first buffer to play
    // The get_buffer function will actually process that data

    if (audiosample_sample_rate(sample) != self->sample_rate) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_sample_rate);
    }
    if (audiosample_channel_count(sample) != self->channel_count) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_channel_count);
    }
    if (audiosample_bits_per_sample(sample) != self->bits_per_sample) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_bits_per_sample);
    }
    bool single_buffer;
    bool samples_signed;
    uint32_t max_buffer_length;
    uint8_t spacing;
    audiosample_get_buffer_structure(sample, false, &single_buffer, &samples_signed, &max_buffer_length, &spacing);
    if (samples_signed != self->samples_signed) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_signedness);
    }

    self->sample = sample;
    self->loop = loop;

    audiosample_reset_buffer(self->sample, false, 0);
    audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);

    // Track remaining sample length in terms of bytes per sample
    self->sample_buffer_length /= (self->bits_per_sample / 8);
    // Store if we have more data in the sample to retrieve
    self->more_data = result == GET_BUFFER_MORE_DATA;

    return;
}

void common_hal_audiofilters_filter_stop(audiofilters_filter_obj_t *self) {
    // When the sample is set to stop playing do any cleanup here
    self->sample = NULL;
    return;
}

#define RANGE_LOW_16 (-28000)
#define RANGE_HIGH_16 (28000)
#define RANGE_SHIFT_16 (16)
#define RANGE_SCALE_16 (0xfffffff / (32768 * 2 - RANGE_HIGH_16)) // 2 for echo+sample

// dynamic range compression via a downward compressor with hard knee
//
// When the output value is within the range +-28000 (about 85% of full scale),
// it is unchanged. Otherwise, it undergoes a gain reduction so that the
// largest possible values, (+32768,-32767) * 2 (2 for echo and sample),
// still fit within the output range
//
// This produces a much louder overall volume with multiple voices, without
// much additional processing.
//
// https://en.wikipedia.org/wiki/Dynamic_range_compression
static
int16_t mix_down_sample(int32_t sample) {
    if (sample < RANGE_LOW_16) {
        sample = (((sample - RANGE_LOW_16) * RANGE_SCALE_16) >> RANGE_SHIFT_16) + RANGE_LOW_16;
    } else if (sample > RANGE_HIGH_16) {
        sample = (((sample - RANGE_HIGH_16) * RANGE_SCALE_16) >> RANGE_SHIFT_16) + RANGE_HIGH_16;
    }
    return sample;
}

audioio_get_buffer_result_t audiofilters_filter_get_buffer(audiofilters_filter_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {
    (void)channel;

    if (!single_channel_output) {
        channel = 0;
    }

    // get the effect values we need from the BlockInput. These may change at run time so you need to do bounds checking if required
    mp_float_t mix = MIN(1.0, MAX(synthio_block_slot_get(&self->mix), 0.0));

    // Switch our buffers to the other buffer
    self->last_buf_idx = !self->last_buf_idx;

    // If we are using 16 bit samples we need a 16 bit pointer, 8 bit needs an 8 bit pointer
    int16_t *word_buffer = (int16_t *)self->buffer[self->last_buf_idx];
    int8_t *hword_buffer = self->buffer[self->last_buf_idx];
    uint32_t length = self->buffer_len / (self->bits_per_sample / 8);

    // Loop over the entire length of our buffer to fill it, this may require several calls to get data from the sample
    while (length != 0) {
        // Check if there is no more sample to play, we will either load more data, reset the sample if loop is on or clear the sample
        if (self->sample_buffer_length == 0) {
            if (!self->more_data) { // The sample has indicated it has no more data to play
                if (self->loop && self->sample) { // If we are supposed to loop reset the sample to the start
                    audiosample_reset_buffer(self->sample, false, 0);
                } else { // If we were not supposed to loop the sample, stop playing it
                    self->sample = NULL;
                }
            }
            if (self->sample) {
                // Load another sample buffer to play
                audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);
                // Track length in terms of words.
                self->sample_buffer_length /= (self->bits_per_sample / 8);
                self->more_data = result == GET_BUFFER_MORE_DATA;
            }
        }

        if (self->sample == NULL) {
            if (self->samples_signed) {
                memset(word_buffer, 0, length * (self->bits_per_sample / 8));
            } else {
                // For unsigned samples set to the middle which is "quiet"
                if (MP_LIKELY(self->bits_per_sample == 16)) {
                    uint16_t *uword_buffer = (uint16_t *)word_buffer;
                    while (length--) {
                        *uword_buffer++ = 32768;
                    }
                } else {
                    memset(hword_buffer, 128, length * (self->bits_per_sample / 8));
                }
            }

            length = 0;
        } else {
            // we have a sample to play and filter
            // Determine how many bytes we can process to our buffer, the less of the sample we have left and our buffer remaining
            uint32_t n = MIN(self->sample_buffer_length, length);

            int16_t *sample_src = (int16_t *)self->sample_remaining_buffer; // for 16-bit samples
            int8_t *sample_hsrc = (int8_t *)self->sample_remaining_buffer; // for 8-bit samples

            if (mix <= 0.01 || !self->filter_states) { // if mix is zero pure sample only or no biquad filter objects are provided
                for (uint32_t i = 0; i < n; i++) {
                    if (MP_LIKELY(self->bits_per_sample == 16)) {
                        word_buffer[i] = sample_src[i];
                    } else {
                        hword_buffer[i] = sample_hsrc[i];
                    }
                }
            } else {
                uint32_t i = 0;
                while (i < n) {
                    uint32_t n_samples = MIN(SYNTHIO_MAX_DUR, n - i);

                    // Fill filter buffer with samples
                    for (uint32_t j = 0; j < n_samples; j++) {
                        if (MP_LIKELY(self->bits_per_sample == 16)) {
                            self->filter_buffer[j] = sample_src[i + j];
                        } else {
                            if (self->samples_signed) {
                                self->filter_buffer[j] = sample_hsrc[i + j];
                            } else {
                                // Be careful here changing from an 8 bit unsigned to signed into a 32-bit signed
                                self->filter_buffer[j] = (int8_t)(((uint8_t)sample_hsrc[i + j]) ^ 0x80);
                            }
                        }
                    }

                    // Process biquad filters
                    for (uint8_t j = 0; j < self->filter_states_len; j++) {
                        synthio_biquad_filter_samples(&self->filter_states[j], self->filter_buffer, n_samples);
                    }

                    // Mix processed signal with original sample and transfer to output buffer
                    for (uint32_t j = 0; j < n_samples; j++) {
                        if (MP_LIKELY(self->bits_per_sample == 16)) {
                            word_buffer[i + j] = mix_down_sample((int32_t)((sample_src[i + j] * (MICROPY_FLOAT_CONST(1.0) - mix)) + (self->filter_buffer[j] * mix)));
                            if (!self->samples_signed) {
                                word_buffer[i + j] ^= 0x8000;
                            }
                        } else {
                            if (self->samples_signed) {
                                hword_buffer[i + j] = (int8_t)((sample_hsrc[i + j] * (1.0 - mix)) + (self->filter_buffer[j] * mix));
                            } else {
                                hword_buffer[i + j] = (uint8_t)(((int8_t)(((uint8_t)sample_hsrc[i + j]) ^ 0x80) * (MICROPY_FLOAT_CONST(1.0) - mix)) + (self->filter_buffer[j] * mix)) ^ 0x80;
                            }
                        }
                    }

                    i += n_samples;
                }
            }

            // Update the remaining length and the buffer positions based on how much we wrote into our buffer
            length -= n;
            word_buffer += n;
            hword_buffer += n;
            self->sample_remaining_buffer += (n * (self->bits_per_sample / 8));
            self->sample_buffer_length -= n;
        }
    }

    // Finally pass our buffer and length to the calling audio function
    *buffer = (uint8_t *)self->buffer[self->last_buf_idx];
    *buffer_length = self->buffer_len;

    // Filter always returns more data but some effects may return GET_BUFFER_DONE or GET_BUFFER_ERROR (see audiocore/__init__.h)
    return GET_BUFFER_MORE_DATA;
}

void audiofilters_filter_get_buffer_structure(audiofilters_filter_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed, uint32_t *max_buffer_length, uint8_t *spacing) {

    // Return information about the effect's buffer (not the sample's)
    // These are used by calling audio objects to determine how to handle the effect's buffer
    *single_buffer = false;
    *samples_signed = self->samples_signed;
    *max_buffer_length = self->buffer_len;
    if (single_channel_output) {
        *spacing = self->channel_count;
    } else {
        *spacing = 1;
    }
}
