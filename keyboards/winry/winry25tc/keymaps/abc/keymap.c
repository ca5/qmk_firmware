#include QMK_KEYBOARD_H
#include "quantum/midi/midi.h"
#include "quantum/midi/qmk_midi.h"

// clang-format off

// -- Color definitions
#define PALE_GREEN_1 (hsv_t){85, 150, 30}
#define PALE_GREEN_2 (hsv_t){95, 150, 30}

const hsv_t rainbow_colors[8] = {
    {0, 255, 255}, {21, 255, 255}, {42, 255, 255}, {85, 255, 255},
    {128, 255, 255}, {170, 255, 255}, {192, 255, 255}, {234, 255, 255}
};

const hsv_t rainbow_colors_dim[8] = {
    {0, 255, 30}, {21, 255, 30}, {42, 255, 30}, {85, 255, 30},
    {128, 255, 30}, {170, 255, 30}, {192, 255, 30}, {234, 255, 30}
};

// New colors for MIDI_CCS layer
#define CC_CH1_COLOR (hsv_t){170, 255, 255} // Blue
#define CC_CH2_COLOR (hsv_t){0, 255, 255}   // Red
#define CC_SET_COLOR (hsv_t){42, 255, 255}  // Yellow
#define LAYER_SWITCH_COLOR (hsv_t){0, 0, 255} // White

// -- LED index mapping
const uint8_t outer_leds_clockwise[16] = {20, 21, 22, 23, 24, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
const uint8_t inner_cc_leds_clockwise[8] = {6, 7, 8, 1, 2, 3, 4, 5};

// clang-format on

static uint8_t outer_led_states[16] = {0};
static uint8_t current_cc_set = 0;
const uint8_t cc_sets[4][2] = {{1, 2}, {3, 4}, {5, 6}, {7, 8}};

enum my_layers {
    _MIDI_NOTES,
    _MIDI_CCS
};

enum custom_keycodes {
    MIDI_NOTE_0 = SAFE_RANGE,
    MIDI_NOTE_1, MIDI_NOTE_2, MIDI_NOTE_3, MIDI_NOTE_4, MIDI_NOTE_5,
    MIDI_NOTE_6, MIDI_NOTE_7, MIDI_NOTE_8, MIDI_NOTE_9, MIDI_NOTE_10,
    MIDI_NOTE_11, MIDI_NOTE_12, MIDI_NOTE_13, MIDI_NOTE_14, MIDI_NOTE_15,
    MIDI_CC_19, MIDI_CC_21, MIDI_CC_22, MIDI_CC_23, MIDI_CC_24,
    MIDI_CC_SEQ_RESET, MIDI_CC_TIMER_RESET, MIDI_CC_SOFT_RESET,
    L_MIDI_NOTES, L_MIDI_CCS,
    CC_CH_1, CC_CH_2,
    CC_SET_1, CC_SET_2, CC_SET_3, CC_SET_4,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_MIDI_NOTES] = LAYOUT(
        MIDI_NOTE_0,  MIDI_NOTE_1,  MIDI_NOTE_2,  MIDI_NOTE_3,  MIDI_NOTE_4,
        MIDI_NOTE_15, MIDI_CC_SEQ_RESET, MIDI_CC_TIMER_RESET, MIDI_CC_SOFT_RESET, MIDI_NOTE_5,
        MIDI_NOTE_14, MIDI_CC_19,   L_MIDI_CCS,   MIDI_CC_21,   MIDI_NOTE_6,
        MIDI_NOTE_13, MIDI_CC_22,   MIDI_CC_23,   MIDI_CC_24,   MIDI_NOTE_7,
        MIDI_NOTE_12, MIDI_NOTE_11, MIDI_NOTE_10, MIDI_NOTE_9,  MIDI_NOTE_8
    ),
    [_MIDI_CCS] = LAYOUT(
        CC_CH_1, CC_CH_1, CC_SET_1, CC_CH_2, CC_CH_2,
        CC_CH_1, CC_CH_1, CC_SET_2, CC_CH_2, CC_CH_2,
        CC_CH_1, CC_CH_1, L_MIDI_NOTES, CC_CH_2, CC_CH_2,
        CC_CH_1, CC_CH_1, CC_SET_3, CC_CH_2, CC_CH_2,
        CC_CH_1, CC_CH_1, CC_SET_4, CC_CH_2, CC_CH_2
    )
};

const uint8_t remap[25] = {20,21,22,23,24,19,6,7,8,9,18,5,0,1,10,17,4,3,2,11,16,15,14,13,12};
const uint8_t note_to_led[16] = {20,21,22,23,24,9,10,11,12,13,14,15,16,17,18,19};

void set_initial_led_state(void);
void set_ccs_layer_leds(void);

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _MIDI_CCS:
            set_ccs_layer_leds();
            break;
        default:
            set_initial_led_state();
            break;
    }
    return state;
}

void set_initial_led_state(void) {
    for (uint8_t i = 0; i < 16; i++) {
        rgblight_sethsv_at((i % 2 == 0) ? PALE_GREEN_1.h : PALE_GREEN_2.h, 150, 30, outer_leds_clockwise[i]);
    }
    for (uint8_t i = 0; i < 8; i++) {
        rgblight_sethsv_at(rainbow_colors_dim[i].h, rainbow_colors_dim[i].s, rainbow_colors_dim[i].v, inner_cc_leds_clockwise[i]);
    }
    rgblight_sethsv_at(0, 0, 100, 0);
    rgblight_set();
}

void set_ccs_layer_leds(void) {
    for (uint8_t row = 0; row < 5; row++) {
        for (uint8_t col = 0; col < 5; col++) {
            uint8_t led_index = remap[(row * 5) + col];
            if (col < 2) {
                rgblight_sethsv_at(CC_CH1_COLOR.h, CC_CH1_COLOR.s, CC_CH1_COLOR.v, led_index);
            } else if (col > 2) {
                rgblight_sethsv_at(CC_CH2_COLOR.h, CC_CH2_COLOR.s, CC_CH2_COLOR.v, led_index);
            } else { // col == 2
                if (row == 2) {
                    rgblight_sethsv_at(LAYER_SWITCH_COLOR.h, LAYER_SWITCH_COLOR.s, LAYER_SWITCH_COLOR.v, led_index);
                } else {
                    rgblight_sethsv_at(CC_SET_COLOR.h, CC_SET_COLOR.s, CC_SET_COLOR.v, led_index);
                }
            }
        }
    }
    rgblight_set();
}

void midi_note_on_user(MidiDevice* device, uint8_t channel, uint8_t note, uint8_t velocity) {
    if (get_highest_layer(layer_state) != _MIDI_NOTES) return;
    uint8_t note_index;
    if (note < 16) { note_index = note; outer_led_states[note_index] |= 1; }
    else if (note >= 32 && note < 48) { note_index = note - 32; outer_led_states[note_index] |= 2; }
    else return;

    uint8_t led_id = note_to_led[note_index];
    uint8_t state  = outer_led_states[note_index];

    if (state == 3) rgblight_setrgb_at(RGB_GREEN, led_id);
    else if (state == 2) rgblight_sethsv_at(0, 0, 100, led_id);
    else if (state == 1) rgblight_sethsv_at(85, 255, 190, led_id);
    rgblight_set();
}

int8_t find_index_in_array(uint8_t value, const uint8_t* array, uint8_t size) {
    for (uint8_t i = 0; i < size; i++) if (array[i] == value) return i;
    return -1;
}

void midi_note_off_user(MidiDevice* device, uint8_t channel, uint8_t note, uint8_t velocity) {
    if (get_highest_layer(layer_state) != _MIDI_NOTES) return;
    uint8_t note_index;
    if (note < 16) { note_index = note; outer_led_states[note_index] &= ~1; }
    else if (note >= 32 && note < 48) { note_index = note - 32; outer_led_states[note_index] &= ~2; }
    else return;

    uint8_t led_id = note_to_led[note_index];
    uint8_t state  = outer_led_states[note_index];

    if (state == 0) {
        int8_t clockwise_index = find_index_in_array(led_id, outer_leds_clockwise, 16);
        if (clockwise_index != -1) {
            rgblight_sethsv_at((clockwise_index % 2 == 0) ? PALE_GREEN_1.h : PALE_GREEN_2.h, 150, 30, led_id);
        }
    } else if (state == 1) rgblight_setrgb_at(RGB_GREEN, led_id);
    else if (state == 2) rgblight_sethsv_at(0, 0, 80, led_id);
    rgblight_set();
}

void keyboard_post_init_user(void) {
    rgblight_enable_noeeprom();
    rgblight_mode(RGBLIGHT_MODE_STATIC_LIGHT);
    set_initial_led_state();
    midi_register_noteon_callback(&midi_device, midi_note_on_user);
    midi_register_noteoff_callback(&midi_device, midi_note_off_user);
}

uint8_t get_cc_value(uint8_t col, uint8_t row) {
    int8_t index = -1;
    if (col == 0) index = 4 - row;
    else if (col == 1) index = 5 + (4 - row);
    else if (col == 3) index = 4 - row;
    else if (col == 4) index = 5 + (4 - row);
    return (index != -1) ? (uint8_t)((index * 127) / 9) : 0;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint8_t channel = 0, velocity = 127;
    uint8_t led_index = remap[(record->event.key.row * 5) + record->event.key.col];
    int8_t clockwise_index = -1;
    uint16_t cc_num = 0;

    if (record->event.pressed) {
        switch (keycode) {
            case L_MIDI_NOTES:
                layer_move(_MIDI_NOTES);
                return false;
            case L_MIDI_CCS:
                layer_move(_MIDI_CCS);
                return false;
            case CC_CH_1:
                if (layer_state_is(_MIDI_CCS)) {
                    midi_send_cc(&midi_device, channel, cc_sets[current_cc_set][0], get_cc_value(record->event.key.col, record->event.key.row));
                }
                return false;
            case CC_CH_2:
                if (layer_state_is(_MIDI_CCS)) {
                    midi_send_cc(&midi_device, channel, cc_sets[current_cc_set][1], get_cc_value(record->event.key.col, record->event.key.row));
                }
                return false;
            case CC_SET_1: current_cc_set = 0; return false;
            case CC_SET_2: current_cc_set = 1; return false;
            case CC_SET_3: current_cc_set = 2; return false;
            case CC_SET_4: current_cc_set = 3; return false;
        }
    }

    switch (keycode) {
        case MIDI_NOTE_0 ... MIDI_NOTE_15:
            if (record->event.pressed) midi_send_noteon(&midi_device, channel, keycode - MIDI_NOTE_0, velocity);
            else midi_send_noteoff(&midi_device, channel, keycode - MIDI_NOTE_0, 0);
            return false;
        case MIDI_CC_19:
        case MIDI_CC_21 ... MIDI_CC_24:
            cc_num = keycode - MIDI_CC_19 + 19;
            break;
        case MIDI_CC_SEQ_RESET: cc_num = 93; break;
        case MIDI_CC_TIMER_RESET: cc_num = 106; break;
        case MIDI_CC_SOFT_RESET: cc_num = 97; break;
    }

    if (cc_num > 0) {
        clockwise_index = find_index_in_array(led_index, inner_cc_leds_clockwise, 8);
        if (clockwise_index != -1) {
            hsv_t c = record->event.pressed ? rainbow_colors[clockwise_index] : rainbow_colors_dim[clockwise_index];
            rgblight_sethsv_at(c.h, c.s, c.v, led_index);
            midi_send_cc(&midi_device, channel, cc_num, record->event.pressed ? velocity : 0);
            rgblight_set();
        }
        return false;
    }

    return true;
}
