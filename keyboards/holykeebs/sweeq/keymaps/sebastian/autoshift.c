// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "qmk_settings.h"

bool vial_get_auto_shifted_key(uint16_t keycode, keyrecord_t *record);

bool get_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
    if (IS_QK_MOD_TAP(keycode)) {
        uint16_t tap_keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
        // Leave symbol mod-taps to the keymap's custom tap/shift overrides.
        if (!QS_auto_shift_enable || tap_keycode < KC_A || tap_keycode > KC_Z) {
            return false;
        }
        keycode = tap_keycode;
    }
    return vial_get_auto_shifted_key(keycode, record);
}
