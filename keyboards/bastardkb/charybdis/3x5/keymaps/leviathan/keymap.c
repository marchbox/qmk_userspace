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

#ifdef POINTING_DEVICE_ENABLE
#    include "bk_pointing_device.h"
#endif

enum charybdis_keymap_layers {
    LAYER_DEF = 0,
    LAYER_RAT,
    LAYER_SYM,
    LAYER_NAV,
    LAYER_FUN,
};

enum custom_keycodes {
    NO_CHEESE = SAFE_RANGE, // Sleep on macOS: Ctrl+Cmd+Q, wait, Esc
};

enum tap_dance_actions {
    TD_SCRSHOT, // Tap: Cmd+Ctrl+Shift+4 (clipboard). Double-tap: Cmd+Shift+5 (options).
};

// Tap dance: screenshot (custom fn so we can send modded keycodes).
static void td_screenshot_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 2) {
        tap_code16(LSG(KC_5));
    } else {
        tap_code16(LCS(LGUI(KC_4)));
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_SCRSHOT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_screenshot_finished, NULL),
};

void keyboard_post_init_user(void) {
    // Enable auto pointer
    set_auto_mouse_enable(true);
    // Inverse scrolling
    // if (!bkpd_get_dragscroll_axis_invert_x()) {
    //     bkpd_set_dragscroll_axis_invert_x(true);
    // }
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

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

typedef struct {
    uint16_t tap_hold_keycode; // The LT()/MT() keycode as it appears in the keymap
    uint16_t tap_keycode;      // The keycode to emit on tap
    uint16_t shifted_keycode;  // Optional: emitted instead when shift is held (0 = none)
} tap_hold_override_t;

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

static const tap_hold_override_t tap_hold_overrides[] = {
    {LALT_T(KC_PLUS),     KC_PLUS,  0},
    {LCTL_T(KC_EQL),      KC_EQL,   KC_PERC},
    {LGUI_T(KC_DQT),      KC_DQT,   KC_QUOT},
    {RGUI_T(KC_LPRN),     KC_LPRN,  0},
    {RCTL_T(KC_RPRN),     KC_RPRN,  KC_AT},
    {RALT_T(KC_LT),       KC_LT,    0},
    {RSFT_T(KC_GT),       KC_GT,    0},
};

// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == NO_CHEESE) {
        if (record->event.pressed) {
            tap_code16(LCTL(LGUI(KC_Q)));
            wait_ms(1000);
            tap_code(KC_ESC);
        }
        return false;
    }
    for (uint8_t i = 0; i < ARRAY_SIZE(tap_hold_overrides); i++) {
        if (keycode == tap_hold_overrides[i].tap_hold_keycode) {
            if (record->tap.count && record->event.pressed) {
                uint16_t shifted = tap_hold_overrides[i].shifted_keycode;
                if (shifted && (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) {
                    uint8_t saved_mods    = get_mods();
                    uint8_t saved_os_mods = get_oneshot_mods();
                    del_mods(MOD_MASK_SHIFT);
                    del_oneshot_mods(MOD_MASK_SHIFT);
                    tap_code16(shifted);
                    set_mods(saved_mods);
                    set_oneshot_mods(saved_os_mods);
                } else {
                    tap_code16(tap_hold_overrides[i].tap_keycode);
                }
                return false;
            }
            break;
        }
    }
    return true;
}

// clang-format off

#define LAYOUT_wrapper(...) LAYOUT(__VA_ARGS__)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_DEF] = LAYOUT_wrapper(
        // row 1
        KC_Q, KC_W, KC_F, KC_P, KC_B,
        KC_J, KC_L, KC_U, KC_Y, KC_MINS,
        // row 2
        HRML(KC_A, KC_R, KC_S, KC_T), KC_G,
        KC_M, HRMR(KC_N, KC_E, KC_I, KC_O),
        // row 3
        KC_Z, KC_X, KC_C, KC_D, KC_V,
        KC_K, KC_H, KC_COMM, KC_DOT, RSQT,
        // thumbs
        DRGSCRL, LT(LAYER_SYM, KC_SPC), OS_LSFT,
        OS_RCTL, LT(LAYER_NAV, KC_BSPC)
    ),
    [LAYER_RAT] = LAYOUT_wrapper(
        // row 1
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        // row 2
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        // row 3
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, MS_BTN1, MS_BTN2, DRGSCRL, KC_TRNS,
        // thumbs
        KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS
    ),
    [LAYER_SYM] = LAYOUT_wrapper(
        // row 1
        KC_1, KC_2, KC_3, KC_4, KC_5,
        KC_6, KC_7, KC_8, KC_9, KC_0,
        // row 2
        HRML(KC_MINS, KC_PLUS, KC_EQL, KC_DQT), KC_COLN,
        KC_SCLN, HRMR(KC_LPRN, KC_RPRN, KC_LT, KC_GT),
        // row 3
        KC_ASTR, KC_DLR, KC_AMPR, KC_EXLM, KC_GRV,
        KC_SLSH, KC_LCBR, KC_RCBR, KC_LBRC, KC_RBRC,
        // thumbs
        KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS
    ),
    [LAYER_NAV] = LAYOUT_wrapper(
        // row 1
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_PGDN, KC_PGUP, KC_NO, KC_NO,
        // row 2
        KC_LSFT, KC_LALT, KC_LCTL, KC_LGUI, KC_NO,
        KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_NO,
        // row 3
        G(KC_Z), G(KC_X), G(KC_C), G(KC_V), LSAG(KC_V),
        KC_NO, KC_END, KC_HOME, KC_NO, KC_NO,
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
        KC_F12, HRMR(KC_VOLD, KC_MUTE, KC_VOLU, KC_NO),
        // row 3
        QK_BOOT, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_MPRV, KC_MPLY, KC_MNXT, QK_BOOT,
        // thumbs
        KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS
    ),
};

const uint16_t PROGMEM combo_def_1_2[] = {KC_W, KC_F, COMBO_END};
const uint16_t PROGMEM combo_sym_1_2[] = {KC_2, KC_3, COMBO_END};
const uint16_t PROGMEM combo_def_11_12[] = {LALT_T(KC_R), LCTL_T(KC_S), COMBO_END};
const uint16_t PROGMEM combo_sym_11_12[] = {LALT_T(KC_PLUS), LCTL_T(KC_EQL), COMBO_END};
const uint16_t PROGMEM combo_def_21_22[] = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM combo_sym_21_22[] = {KC_DLR, KC_AMPR, COMBO_END};
const uint16_t PROGMEM combo_def_21_22_23[] = {KC_X, KC_C, KC_D, COMBO_END};
const uint16_t PROGMEM combo_sym_21_22_23[] = {KC_DLR, KC_AMPR, KC_EXLM, COMBO_END};
const uint16_t PROGMEM combo_def_7_8[] = {KC_U, KC_Y, COMBO_END};
const uint16_t PROGMEM combo_sym_7_8[] = {KC_8, KC_9, COMBO_END};
const uint16_t PROGMEM combo_def_17_18[] = {RCTL_T(KC_E), RALT_T(KC_I), COMBO_END};
const uint16_t PROGMEM combo_sym_17_18[] = {RCTL_T(KC_RPRN), RALT_T(KC_LT), COMBO_END};
const uint16_t PROGMEM combo_def_27_28[] = {KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM combo_sym_27_28[] = {KC_RCBR, KC_LBRC, COMBO_END};
const uint16_t PROGMEM combo_def_26_27_28[] = {KC_H, KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM combo_sym_26_27_28[] = {KC_LCBR, KC_RCBR, KC_LBRC, COMBO_END};
const uint16_t PROGMEM combo_def_0_1_2_3[] = {KC_Q, KC_W, KC_F, KC_P, COMBO_END};
const uint16_t PROGMEM combo_def_6_7_8_9[] = {KC_L, KC_U, KC_Y, KC_MINS, COMBO_END};
const uint16_t PROGMEM combo_def_20_21_22_23[] = {KC_Z, KC_X, KC_C, KC_D, COMBO_END};
const uint16_t PROGMEM combo_def_26_27_28_29[] = {KC_H, KC_COMM, KC_DOT, RSQT, COMBO_END};

combo_t key_combos[] = {
    COMBO(combo_def_1_2, KC_ESC),
    COMBO(combo_sym_1_2, KC_ESC),
    COMBO(combo_def_11_12, KC_TAB),
    COMBO(combo_sym_11_12, KC_TAB),
    COMBO(combo_def_21_22, CW_TOGG),
    COMBO(combo_sym_21_22, CW_TOGG),
    COMBO(combo_def_7_8, KC_DEL),
    COMBO(combo_sym_7_8, KC_DEL),
    COMBO(combo_def_17_18, KC_BSPC),
    COMBO(combo_sym_17_18, KC_BSPC),
    COMBO(combo_def_27_28, KC_ENT),
    COMBO(combo_sym_27_28, KC_ENT),
    COMBO(combo_def_20_21_22_23, G(KC_SPC)),
    COMBO(combo_def_26_27_28_29, KC_MUTE),
    COMBO(combo_def_21_22_23, KC_SPC),
    COMBO(combo_sym_21_22_23, KC_SPC),
    COMBO(combo_def_26_27_28, KC_SPC),
    COMBO(combo_sym_26_27_28, KC_SPC),
    COMBO(combo_def_0_1_2_3, NO_CHEESE),
    COMBO(combo_def_6_7_8_9, TD(TD_SCRSHOT)),
};

const key_override_t ko_comm_ques = ko_make_basic(MOD_MASK_SHIFT, KC_COMM, KC_QUES);
const key_override_t ko_dot_exlm = ko_make_basic(MOD_MASK_SHIFT, KC_DOT, KC_EXLM);
const key_override_t ko_rsqt_ndsh = ko_make_basic(MOD_MASK_SHIFT, RSQT, NDSH);
const key_override_t ko_eql_perc = ko_make_basic(MOD_MASK_SHIFT, KC_EQL, KC_PERC);
const key_override_t ko_dqt_quot = ko_make_basic(MOD_MASK_SHIFT, KC_DQT, KC_QUOT);
const key_override_t ko_coln_dot = ko_make_basic(MOD_MASK_SHIFT, KC_COLN, KC_DOT);
const key_override_t ko_astr_circ = ko_make_basic(MOD_MASK_SHIFT, KC_ASTR, KC_CIRC);
const key_override_t ko_dlr_hash = ko_make_basic(MOD_MASK_SHIFT, KC_DLR, KC_HASH);
const key_override_t ko_ampr_pipe = ko_make_basic(MOD_MASK_SHIFT, KC_AMPR, KC_PIPE);
const key_override_t ko_exlm_ques = ko_make_basic(MOD_MASK_SHIFT, KC_EXLM, KC_QUES);
const key_override_t ko_scln_comm = ko_make_basic(MOD_MASK_SHIFT, KC_SCLN, KC_COMM);
const key_override_t ko_rprn_at = ko_make_basic(MOD_MASK_SHIFT, KC_RPRN, KC_AT);
const key_override_t ko_slsh_bsls = ko_make_basic(MOD_MASK_SHIFT, KC_SLSH, KC_BSLS);
const key_override_t ko_lcbr_ldqt = ko_make_basic(MOD_MASK_SHIFT, KC_LCBR, LDQT);
const key_override_t ko_rcbr_rdqt = ko_make_basic(MOD_MASK_SHIFT, KC_RCBR, RDQT);
const key_override_t ko_lbrc_lsqt = ko_make_basic(MOD_MASK_SHIFT, KC_LBRC, LSQT);
const key_override_t ko_rbrc_rsqt = ko_make_basic(MOD_MASK_SHIFT, KC_RBRC, RSQT);

// This globally defines all key overrides to be used
const key_override_t *key_overrides[] = {
    &ko_comm_ques,
    &ko_dot_exlm,
    &ko_rsqt_ndsh,
    &ko_eql_perc,
    &ko_dqt_quot,
    &ko_coln_dot,
    &ko_astr_circ,
    &ko_dlr_hash,
    &ko_ampr_pipe,
    &ko_exlm_ques,
    &ko_scln_comm,
    &ko_rprn_at,
    &ko_slsh_bsls,
    &ko_lcbr_ldqt,
    &ko_rcbr_rdqt,
    &ko_lbrc_lsqt,
    &ko_rbrc_rsqt,
};

// clang-format on
