/* Copyright 2021 Andrzej Ressel (andrzej.ressel@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "quantum/midi/midi.h"
#include "quantum/midi/qmk_midi.h"

// clang-format off

// -- Color definitions
// Outer buttons - inactive
#define PALE_GREEN_1 (hsv_t){85, 150, 30}
#define PALE_GREEN_2 (hsv_t){95, 150, 30}

// Inner buttons - active (bright rainbow)
const hsv_t rainbow_colors[8] = {
    {0, 255, 255},   // Red
    {21, 255, 255},  // Orange
    {42, 255, 255},  // Yellow
    {85, 255, 255},  // Green
    {128, 255, 255}, // Cyan
    {170, 255, 255}, // Blue
    {192, 255, 255}, // Purple
    {234, 255, 255}  // Pink
};

// Inner buttons - inactive (dim rainbow)
const hsv_t rainbow_colors_dim[8] = {
    {0, 255, 30},
    {21, 255, 30},
    {42, 255, 30},
    {85, 255, 30},
    {128, 255, 30},
    {170, 255, 30},
    {192, 255, 30},
    {234, 255, 30}
};

// -- LED index mapping
// Outer buttons, clockwise from top-left
const uint8_t outer_leds_clockwise[16] = {20, 21, 22, 23, 24, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
// Inner CC buttons, clockwise from top-left
const uint8_t inner_cc_leds_clockwise[8] = {6, 7, 8, 1, 2, 3, 4, 5};

// clang-format on

enum my_layers {
    _MIDI_LAYER,
    _NUMPAD_LAYER,
    _FUNCTION_LAYER
};

enum custom_keycodes {
    MIDI_NOTE_0 = SAFE_RANGE,
    MIDI_NOTE_1,
    MIDI_NOTE_2,
    MIDI_NOTE_3,
    MIDI_NOTE_4,
    MIDI_NOTE_5,
    MIDI_NOTE_6,
    MIDI_NOTE_7,
    MIDI_NOTE_8,
    MIDI_NOTE_9,
    MIDI_NOTE_10,
    MIDI_NOTE_11,
    MIDI_NOTE_12,
    MIDI_NOTE_13,
    MIDI_NOTE_14,
    MIDI_NOTE_15,
    MIDI_CC_19,
    MIDI_CC_21,
    MIDI_CC_22,
    MIDI_CC_23,
    MIDI_CC_24,
    MIDI_CC_SEQ_RESET, // CC 93
    MIDI_CC_TIMER_RESET, // CC 106
    MIDI_CC_SOFT_RESET, // CC 97
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_MIDI_LAYER] = LAYOUT(
        MIDI_NOTE_0,  MIDI_NOTE_1,  MIDI_NOTE_2,  MIDI_NOTE_3,  MIDI_NOTE_4,
        MIDI_NOTE_15, MIDI_CC_SEQ_RESET,   MIDI_CC_TIMER_RESET,   MIDI_CC_SOFT_RESET,   MIDI_NOTE_5,
        MIDI_NOTE_14, MIDI_CC_19,   TG(_NUMPAD_LAYER), MIDI_CC_21,   MIDI_NOTE_6,
        MIDI_NOTE_13, MIDI_CC_22,   MIDI_CC_23,   MIDI_CC_24,   MIDI_NOTE_7,
        MIDI_NOTE_12, MIDI_NOTE_11, MIDI_NOTE_10, MIDI_NOTE_9,  MIDI_NOTE_8
    ),
    [_NUMPAD_LAYER] = LAYOUT(
        KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS, KC_ESC,
        KC_P7,   KC_P8,   KC_P9,   KC_PPLS, UG_TOGG,
        KC_P4,   TG(_MIDI_LAYER),   KC_P6,   KC_PENT, UG_NEXT,
        KC_P1,   KC_P2,   KC_P3,   KC_UP,   MO(_FUNCTION_LAYER),
        KC_P0,   KC_PDOT, KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_FUNCTION_LAYER] = LAYOUT(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,
        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
        KC_F11,  KC_F12,  KC_MUTE, KC_VOLD, KC_VOLU,
        KC_MPLY, KC_MSTP, KC_MPRV, KC_MNXT, _______,
        _______, _______, _______, MI_OCTU, MI_OCTD
    ),
};

const uint8_t remap[25] = {
    20,21,22,23,24,
    19,6,7,8,9,
    18,5,0,1,10,
    17,4,3,2,11,
    16,15,14,13,12,
};

const uint8_t note_to_led[16] = {
    20, 21, 22, 23, 24, // 0-4
    9, 10, 11, 12, // 5-8
    13, 14, 15, 16, // 9-12
    17, 18, 19 // 13-15
};

void set_initial_led_state(void) {
    // Set outer button inactive colors
    for (uint8_t i = 0; i < 16; i++) {
        if (i % 2 == 0) {
            rgblight_sethsv_at(PALE_GREEN_1.h, PALE_GREEN_1.s, PALE_GREEN_1.v, outer_leds_clockwise[i]);
        } else {
            rgblight_sethsv_at(PALE_GREEN_2.h, PALE_GREEN_2.s, PALE_GREEN_2.v, outer_leds_clockwise[i]);
        }
    }

    // Set inner button inactive colors
    for (uint8_t i = 0; i < 8; i++) {
        hsv_t c = rainbow_colors_dim[i];
        rgblight_sethsv_at(c.h, c.s, c.v, inner_cc_leds_clockwise[i]);
    }

    // Set the TG button (LED 0) to dim white
    rgblight_sethsv_at(0, 0, 80, 0);

    rgblight_set();
}

void midi_note_on_user(MidiDevice* device, uint8_t channel, uint8_t note, uint8_t velocity) {
    if (note >= 0 && note < 16) {
        // Green for notes 0-15
        rgblight_setrgb_at(RGB_GREEN, note_to_led[note]);
        rgblight_set();
    } else if (note >= 32 && note < 48) {
        // White for notes 32-47
        rgblight_setrgb_at(RGB_WHITE, note_to_led[note - 32]);
        rgblight_set();
    }
}

// Helper to find the index of a value in an array
int8_t find_index_in_array(uint8_t value, const uint8_t* array, uint8_t size) {
    for (uint8_t i = 0; i < size; i++) {
        if (array[i] == value) {
            return i;
        }
    }
    return -1; // Not found
}

void midi_note_off_user(MidiDevice* device, uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t led_id = 0;
    if (note >= 0 && note < 16) {
        led_id = note_to_led[note];
    } else if (note >= 32 && note < 48) {
        led_id = note_to_led[note - 32];
    } else {
        return; // Not a note we care about
    }

    int8_t clockwise_index = find_index_in_array(led_id, outer_leds_clockwise, 16);
    if (clockwise_index != -1) {
        if (clockwise_index % 2 == 0) {
            rgblight_sethsv_at(PALE_GREEN_1.h, PALE_GREEN_1.s, PALE_GREEN_1.v, led_id);
        } else {
            rgblight_sethsv_at(PALE_GREEN_2.h, PALE_GREEN_2.s, PALE_GREEN_2.v, led_id);
        }
        rgblight_set();
    }
}

void keyboard_post_init_user(void) {
    rgblight_enable_noeeprom();
    rgblight_mode(RGBLIGHT_MODE_STATIC_LIGHT);
    set_initial_led_state();
    midi_register_noteon_callback(&midi_device, midi_note_on_user);
    midi_register_noteoff_callback(&midi_device, midi_note_off_user);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint8_t  channel         = 0; // MIDI channel 1
    uint8_t  velocity        = 127;
    uint8_t  led_index       = remap[(record->event.key.row * 5) + record->event.key.col];
    int8_t   clockwise_index = -1;
    uint16_t cc_num          = 0;

    switch (keycode) {
        case MIDI_NOTE_0 ... MIDI_NOTE_15:
            if (record->event.pressed) {
                midi_send_noteon(&midi_device, channel, keycode - MIDI_NOTE_0, velocity);
            } else {
                midi_send_noteoff(&midi_device, channel, keycode - MIDI_NOTE_0, 0);
            }
            return false; // Skip default processing

        case MIDI_CC_19:
        case MIDI_CC_21 ... MIDI_CC_24:
            cc_num = keycode - MIDI_CC_19 + 19;
            break;
        case MIDI_CC_SEQ_RESET:
            cc_num = 93;
            break;
        case MIDI_CC_TIMER_RESET:
            cc_num = 106;
            break;
        case MIDI_CC_SOFT_RESET:
            cc_num = 97;
            break;
    }

    if (cc_num > 0) {
        clockwise_index = find_index_in_array(led_index, inner_cc_leds_clockwise, 8);
        if (clockwise_index != -1) {
            if (record->event.pressed) {
                hsv_t c = rainbow_colors[clockwise_index];
                rgblight_sethsv_at(c.h, c.s, c.v, led_index);
                midi_send_cc(&midi_device, channel, cc_num, velocity);
            } else {
                hsv_t c = rainbow_colors_dim[clockwise_index];
                rgblight_sethsv_at(c.h, c.s, c.v, led_index);
                midi_send_cc(&midi_device, channel, cc_num, 0);
            }
            rgblight_set();
        }
        return false;
    }

    return true; // Process all other keycodes normally
}
