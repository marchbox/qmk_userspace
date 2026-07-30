/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
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
#include "deferred_exec.h"

#ifdef POINTING_DEVICE_ENABLE
#    include "bk_pointing_device.h"
#endif

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    include "timer.h"
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

enum charybdis_keymap_layers {
    LAYER_DEF = 0,
    LAYER_SYM,
    LAYER_NAV,
    LAYER_FUN,
    LAYER_RAT,
};

enum custom_keycodes {
    NO_CHEESE = SAFE_RANGE,
};

enum tap_dances {
    TD_SCREENSHOT,
};

// Automatically enable sniping-mode on the pointer layer.
#define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_RAT

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD

#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

void keyboard_post_init_user(void) {
    // Enable auto pointer
    set_auto_mouse_enable(true);
    // Inverse scrolling
    if (!bkpd_get_dragscroll_axis_invert_x()) {
        bkpd_set_dragscroll_axis_invert_x(true);
    }
    if (!bkpd_get_dragscroll_axis_invert_y()) {
        bkpd_set_dragscroll_axis_invert_y(true);
    }
}

// Keep the auto-mouse layer active while drag-scrolling.
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
bool is_mouse_record_user(uint16_t keycode, keyrecord_t *record) {
    (void)record;
    return keycode == DRGSCRL;
}
#endif

// Enable tri layer
layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, LAYER_SYM, LAYER_NAV, LAYER_FUN);
}

static uint32_t send_sleep_escape(uint32_t trigger_time, void *cb_arg) {
    (void)trigger_time;
    (void)cb_arg;
    tap_code(KC_ESC);
    return 0;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == NO_CHEESE) {
        if (record->event.pressed) {
            tap_code16(LCTL(LGUI(KC_Q)));
            if (defer_exec(1000, send_sleep_escape, NULL) == INVALID_DEFERRED_TOKEN) {
                tap_code(KC_ESC);
            }
        }
        return false;
    }
    return true;
}

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

// clang-format off

// Symbols
#define LDQT RALT(KC_LBRC)
#define RDQT RSA(KC_LBRC)
#define LSQT RALT(KC_RBRC)
#define RSQT RSA(KC_RBRC)
#define NDSH RALT(KC_MINS)
#define MDSH RSA(KC_MINS)

// Home Row Mods
#define HRML(K1, K2, K3, K4) LSFT_T(K1), LALT_T(K2), LCTL_T(K3), LGUI_T(K4)
#define HRMR(K1, K2, K3, K4) RGUI_T(K1), RCTL_T(K2), RALT_T(K3), RSFT_T(K4)

#define LAYOUT_wrapper(...) LAYOUT(__VA_ARGS__)

tap_dance_action_t tap_dance_actions[] = {
  [TD_SCREENSHOT] = ACTION_TAP_DANCE_DOUBLE(LCTL(LGUI(LSFT(KC_4))), LGUI(LSFT(KC_5))),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_DEF] = LAYOUT_wrapper(
    // row 1
    KC_Q, KC_W, KC_F, KC_P, KC_B,
    KC_J, KC_L, KC_U, KC_Y, KC_MINS,
    // row 2
    HRML(KC_A, KC_R, KC_S, KC_T), KC_G,
    KC_M, HRMR(KC_N, KC_E, KC_I, KC_O),
    // row 3
    LT(LAYER_RAT, KC_Z), KC_X, KC_C, KC_D, KC_V,
    KC_K, KC_H, KC_COMM, KC_DOT, LT(LAYER_RAT, RSQT),
    // thumbs
    OS_LGUI, LT(LAYER_SYM, KC_SPC), OS_LSFT,
    OS_RCTL, LT(LAYER_NAV, KC_BSPC)
  ),
  [LAYER_SYM] = LAYOUT_wrapper(
    // row 1
    KC_1, KC_2, KC_3, KC_4, KC_5,
    KC_6, KC_7, KC_8, KC_9, KC_0,
    // row 2
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    // row 3
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    // thumbs
    KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS
  ),
  [LAYER_NAV] = LAYOUT_wrapper(
    // row 1
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    // row 2
    KC_LSFT, KC_LALT, KC_LCTL, KC_LGUI, KC_NO,
    KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_NO,
    // row 3
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    // thumbs
    KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS
  ),
  [LAYER_FUN] = LAYOUT_wrapper(
    // row 1
    KC_F1, KC_F2, KC_F3, KC_F4, KC_F5,
    KC_F6, KC_F7, KC_F8, KC_F9, KC_F10,
    // row 2
    KC_LSFT, KC_LALT, KC_LCTL, KC_LGUI, KC_F11,
    KC_F12, KC_VOLD, KC_MUTE, KC_VOLU, KC_NO,
    // row 3
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    // thumbs
    KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS
  ),
  [LAYER_RAT] = LAYOUT_wrapper(
    // row 1
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    // row 2
    KC_LSFT, KC_LALT, KC_LCTL, KC_LGUI, KC_NO,
    KC_NO, OS_RGUI, OS_RCTL, OS_RALT, OS_RSFT,
    // row 3
    KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
    KC_NO, MS_BTN1, MS_BTN2, DRGSCRL, KC_NO,
    // thumbs
    KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS
  ),
};

// clang-format on

enum combo_events {
    CMB_BOOT_LEFT,
    CMB_BOOT_RIGHT,
    CMB_NO_CHEESE,
    CMB_SCREENSHOT,
    CMB_SPOTLIGHT,
    CMB_MUTE,
    CMB_ESC,
    CMB_DEL,
    CMB_TAB,
    CMB_BSPC,
    CMB_ENTER,
    CMB_SPACE_LEFT,
    CMB_SPACE_RIGHT,
    CMB_CAPS_WORD,
    CMB_COUNT,
};

const uint16_t PROGMEM combo_boot_left[]   = {KC_Q, KC_W, KC_F, KC_P, COMBO_END};
const uint16_t PROGMEM combo_boot_right[]  = {KC_L, KC_U, KC_Y, KC_MINS, COMBO_END};
const uint16_t PROGMEM combo_no_cheese[]   = {KC_Q, KC_W, KC_F, KC_P, COMBO_END};
const uint16_t PROGMEM combo_screenshot[]  = {KC_L, KC_U, KC_Y, KC_MINS, COMBO_END};
const uint16_t PROGMEM combo_spotlight[]   = {LT(LAYER_RAT, KC_Z), KC_X, KC_C, KC_D, COMBO_END};
const uint16_t PROGMEM combo_mute[]        = {KC_H, KC_COMM, KC_DOT, LT(LAYER_RAT, RSQT), COMBO_END};
const uint16_t PROGMEM combo_esc[]         = {KC_W, KC_F, COMBO_END};
const uint16_t PROGMEM combo_del[]         = {KC_U, KC_Y, COMBO_END};
const uint16_t PROGMEM combo_tab[]         = {LALT_T(KC_R), LCTL_T(KC_S), COMBO_END};
const uint16_t PROGMEM combo_bspc[]        = {RCTL_T(KC_E), RALT_T(KC_I), COMBO_END};
const uint16_t PROGMEM combo_enter[]       = {KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM combo_space_left[]  = {KC_X, KC_C, KC_D, COMBO_END};
const uint16_t PROGMEM combo_space_right[] = {KC_H, KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM combo_caps_word[]   = {KC_X, KC_C, COMBO_END};

combo_t key_combos[] = {
    [CMB_BOOT_LEFT]   = COMBO(combo_boot_left, QK_BOOT),
    [CMB_BOOT_RIGHT]  = COMBO(combo_boot_right, QK_BOOT),
    [CMB_NO_CHEESE]   = COMBO(combo_no_cheese, NO_CHEESE),
    [CMB_SCREENSHOT]  = COMBO(combo_screenshot, TD(TD_SCREENSHOT)),
    [CMB_SPOTLIGHT]   = COMBO(combo_spotlight, LGUI(KC_SPC)),
    [CMB_MUTE]        = COMBO(combo_mute, KC_MUTE),
    [CMB_ESC]         = COMBO(combo_esc, KC_ESC),
    [CMB_DEL]         = COMBO(combo_del, KC_DEL),
    [CMB_TAB]         = COMBO(combo_tab, KC_TAB),
    [CMB_BSPC]        = COMBO(combo_bspc, KC_BSPC),
    [CMB_ENTER]       = COMBO(combo_enter, KC_ENT),
    [CMB_SPACE_LEFT]  = COMBO(combo_space_left, KC_SPC),
    [CMB_SPACE_RIGHT] = COMBO(combo_space_right, KC_SPC),
    [CMB_CAPS_WORD]   = COMBO(combo_caps_word, CW_TOGG),
};

#define COMBO_LAYER_MASK(layer) (UINT32_C(1) << (layer))

static const uint32_t PROGMEM combo_layer_masks[CMB_COUNT] = {
    [CMB_BOOT_LEFT]   = COMBO_LAYER_MASK(LAYER_FUN),
    [CMB_BOOT_RIGHT]  = COMBO_LAYER_MASK(LAYER_FUN),
    [CMB_NO_CHEESE]   = COMBO_LAYER_MASK(LAYER_DEF),
    [CMB_SCREENSHOT]  = COMBO_LAYER_MASK(LAYER_DEF),
    [CMB_SPOTLIGHT]   = COMBO_LAYER_MASK(LAYER_DEF),
    [CMB_MUTE]        = COMBO_LAYER_MASK(LAYER_DEF),
    [CMB_ESC]         = COMBO_LAYER_MASK(LAYER_DEF) | COMBO_LAYER_MASK(LAYER_SYM),
    [CMB_DEL]         = COMBO_LAYER_MASK(LAYER_DEF) | COMBO_LAYER_MASK(LAYER_SYM),
    [CMB_TAB]         = COMBO_LAYER_MASK(LAYER_DEF) | COMBO_LAYER_MASK(LAYER_SYM),
    [CMB_BSPC]        = COMBO_LAYER_MASK(LAYER_DEF) | COMBO_LAYER_MASK(LAYER_SYM),
    [CMB_ENTER]       = COMBO_LAYER_MASK(LAYER_DEF) | COMBO_LAYER_MASK(LAYER_SYM),
    [CMB_SPACE_LEFT]  = COMBO_LAYER_MASK(LAYER_DEF) | COMBO_LAYER_MASK(LAYER_SYM),
    [CMB_SPACE_RIGHT] = COMBO_LAYER_MASK(LAYER_DEF) | COMBO_LAYER_MASK(LAYER_SYM),
    [CMB_CAPS_WORD]   = COMBO_LAYER_MASK(LAYER_DEF),
};

uint8_t combo_ref_from_layer(uint8_t layer) {
    (void)layer;
    return LAYER_DEF;
}

bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    (void)combo;
    (void)keycode;
    (void)record;

    uint32_t allowed_layers = pgm_read_dword(&combo_layer_masks[combo_index]);
    uint8_t  active_layer   = get_highest_layer(layer_state | default_layer_state);
    return allowed_layers & COMBO_LAYER_MASK(active_layer);
}
