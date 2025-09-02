#pragma once
#include_next <config.h>

#define MIDI_ADVANCED

// This flag disables the HID keyboard interface, making the device a pure MIDI controller.
#define USB_DISABLE_HID_KEYBOARD