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
    LAYER_SYM,
    LAYER_NAV,
    LAYER_FUN,
    LAYER_RAT,
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

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

typedef struct {
    uint16_t tap_hold_keycode; // The LT()/MT() keycode as it appears in the keymap
    uint16_t tap_keycode;      // The keycode to emit on tap
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
    {LT(LAYER_RAT, RSQT), RSQT},
    {LSFT_T(KC_PERC),     KC_PERC},
    {LALT_T(KC_MINS),     KC_MINS},
    {LCTL_T(KC_EQL),      KC_EQL},
    {LGUI_T(KC_DQT),      KC_DQT},
    {RGUI_T(KC_LPRN),     KC_LPRN},
    {RCTL_T(KC_LCBR),     KC_LCBR},
    {RALT_T(KC_LBRC),     KC_LBRC},
    {RSFT_T(KC_LT),       KC_LT},
};

// clang-format on

static const key_override_t *autoshift_override(uint16_t keycode);

static uint16_t autoshift_tap_keycode(uint16_t keycode) {
    for (uint8_t i = 0; i < ARRAY_SIZE(tap_hold_overrides); ++i) {
        if (keycode == tap_hold_overrides[i].tap_hold_keycode) {
            return tap_hold_overrides[i].tap_keycode;
        }
    }
    if (IS_QK_MOD_TAP(keycode)) {
        return QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
    }
    if (IS_QK_LAYER_TAP(keycode)) {
        return QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    }
    return keycode;
}

bool get_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
    if (!get_autoshift_state()) {
        return false;
    }
    uint16_t tap_keycode = autoshift_tap_keycode(keycode);
    if (autoshift_override(tap_keycode)) {
        return true;
    }
    if (IS_QK_MODS(tap_keycode)) {
        tap_keycode = QK_MODS_GET_BASIC_KEYCODE(tap_keycode);
    }
    return (tap_keycode >= KC_A && tap_keycode <= KC_Z) || (tap_keycode >= KC_1 && tap_keycode <= KC_0) || (tap_keycode >= KC_MINS && tap_keycode <= KC_SLSH) || tap_keycode == KC_NUBS || tap_keycode == KC_TAB;
}

void autoshift_press_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    keycode                        = autoshift_tap_keycode(keycode);
    const key_override_t *override = shifted ? autoshift_override(keycode) : NULL;
    if (override) {
        del_weak_mods(MOD_MASK_SHIFT);
        register_code16(override->replacement);
    } else {
        if (shifted) {
            add_weak_mods(MOD_BIT(KC_LSFT));
        }
        register_code16(keycode);
    }
}

void autoshift_release_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    keycode                        = autoshift_tap_keycode(keycode);
    const key_override_t *override = shifted ? autoshift_override(keycode) : NULL;
    unregister_code16(override ? override->replacement : keycode);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == NO_CHEESE) {
        if (record->event.pressed) {
            tap_code16(LCTL(LGUI(KC_Q)));
            wait_ms(1000);
            tap_code(KC_ESC);
        }
        return false;
    }
    static bool manual_taps[ARRAY_SIZE(tap_hold_overrides)];
    for (uint8_t i = 0; i < ARRAY_SIZE(tap_hold_overrides); i++) {
        if (keycode == tap_hold_overrides[i].tap_hold_keycode) {
            if (!record->tap.count) {
                return true;
            }
            if (!record->event.pressed) {
                bool manual    = manual_taps[i];
                manual_taps[i] = false;
                return !manual;
            }
            if (get_autoshift_state() && get_auto_shifted_key(keycode, record) && !((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT)) {
                return true;
            }
            manual_taps[i]                 = true;
            const key_override_t *override = autoshift_override(tap_hold_overrides[i].tap_keycode);
            if (override && ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT)) {
                uint8_t saved_mods    = get_mods();
                uint8_t saved_os_mods = get_oneshot_mods();
                del_mods(MOD_MASK_SHIFT);
                del_oneshot_mods(MOD_MASK_SHIFT);
                tap_code16(override->replacement);
                set_mods(saved_mods);
                set_oneshot_mods(saved_os_mods);
            } else {
                tap_code16(tap_hold_overrides[i].tap_keycode);
            }
            return false;
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
        HRML(KC_PERC, KC_MINS, KC_EQL, KC_DQT), KC_COLN,
        KC_SCLN, HRMR(KC_LPRN, KC_LCBR, KC_LBRC, KC_LT),
        // row 3
        KC_ASTR, KC_PIPE, KC_AMPR, KC_EXLM, KC_GRV,
        KC_SLSH, KC_DLR, KC_HASH, LDQT, LSQT,
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

const uint16_t PROGMEM combo_def_1_2[] = {KC_W, KC_F, COMBO_END};
const uint16_t PROGMEM combo_sym_1_2[] = {KC_2, KC_3, COMBO_END};
const uint16_t PROGMEM combo_def_11_12[] = {LALT_T(KC_R), LCTL_T(KC_S), COMBO_END};
const uint16_t PROGMEM combo_sym_11_12[] = {LALT_T(KC_MINS), LCTL_T(KC_EQL), COMBO_END};
const uint16_t PROGMEM combo_def_21_22[] = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM combo_sym_21_22[] = {KC_PIPE, KC_AMPR, COMBO_END};
const uint16_t PROGMEM combo_def_21_22_23[] = {KC_X, KC_C, KC_D, COMBO_END};
const uint16_t PROGMEM combo_sym_21_22_23[] = {KC_PIPE, KC_AMPR, KC_EXLM, COMBO_END};
const uint16_t PROGMEM combo_def_7_8[] = {KC_U, KC_Y, COMBO_END};
const uint16_t PROGMEM combo_sym_7_8[] = {KC_8, KC_9, COMBO_END};
const uint16_t PROGMEM combo_def_17_18[] = {RCTL_T(KC_E), RALT_T(KC_I), COMBO_END};
const uint16_t PROGMEM combo_sym_17_18[] = {RCTL_T(KC_LCBR), RALT_T(KC_LBRC), COMBO_END};
const uint16_t PROGMEM combo_def_27_28[] = {KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM combo_sym_27_28[] = {KC_HASH, LDQT, COMBO_END};
const uint16_t PROGMEM combo_def_26_27_28[] = {KC_H, KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM combo_sym_26_27_28[] = {KC_DLR, KC_HASH, LDQT, COMBO_END};
const uint16_t PROGMEM combo_def_0_1_2_3[] = {KC_Q, KC_W, KC_F, KC_P, COMBO_END};
const uint16_t PROGMEM combo_def_6_7_8_9[] = {KC_L, KC_U, KC_Y, KC_MINS, COMBO_END};
const uint16_t PROGMEM combo_def_20_21_22_23[] = {LT(LAYER_RAT, KC_Z), KC_X, KC_C, KC_D, COMBO_END};
const uint16_t PROGMEM combo_def_26_27_28_29[] = {KC_H, KC_COMM, KC_DOT, LT(LAYER_RAT, RSQT), COMBO_END};

const uint16_t PROGMEM combo_ampr[] = {KC_Q, KC_W, COMBO_END};
const uint16_t PROGMEM combo_coln[] = {KC_F, KC_P, COMBO_END};
const uint16_t PROGMEM combo_astr[] = {KC_J, KC_L, COMBO_END};
const uint16_t PROGMEM combo_scln[] = {KC_L, KC_U, COMBO_END};
const uint16_t PROGMEM combo_pipe[] = {KC_Y, KC_MINS, COMBO_END};
const uint16_t PROGMEM combo_perc[] = {LSFT_T(KC_A), LALT_T(KC_R), COMBO_END};
const uint16_t PROGMEM combo_dqt[] = {LCTL_T(KC_S), LGUI_T(KC_T), COMBO_END};
const uint16_t PROGMEM combo_lbrc[] = {KC_M, RGUI_T(KC_N), COMBO_END};
const uint16_t PROGMEM combo_lprn[] = {RGUI_T(KC_N), RCTL_T(KC_E), COMBO_END};
const uint16_t PROGMEM combo_lt[] = {RALT_T(KC_I), RSFT_T(KC_O), COMBO_END};
const uint16_t PROGMEM combo_dlr[] = {LT(LAYER_RAT, KC_Z), KC_X, COMBO_END};
const uint16_t PROGMEM combo_eql[] = {KC_C, KC_D, COMBO_END};
const uint16_t PROGMEM combo_slsh[] = {KC_K, KC_H, COMBO_END};
const uint16_t PROGMEM combo_lcbr[] = {KC_H, KC_COMM, COMBO_END};
const uint16_t PROGMEM combo_hash[] = {KC_DOT, LT(LAYER_RAT, RSQT), COMBO_END};

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
    COMBO(combo_def_21_22_23, KC_GRV),
    COMBO(combo_sym_21_22_23, KC_SPC),
    COMBO(combo_def_26_27_28, KC_SPC),
    COMBO(combo_sym_26_27_28, KC_SPC),
    COMBO(combo_def_0_1_2_3, NO_CHEESE),
    COMBO(combo_def_6_7_8_9, TD(TD_SCRSHOT)),
    COMBO(combo_ampr, KC_AMPR),
    COMBO(combo_coln, KC_COLN),
    COMBO(combo_astr, KC_ASTR),
    COMBO(combo_scln, KC_SCLN),
    COMBO(combo_pipe, KC_PIPE),
    COMBO(combo_perc, KC_PERC),
    COMBO(combo_dqt, KC_DQT),
    COMBO(combo_lbrc, KC_LBRC),
    COMBO(combo_lprn, KC_LPRN),
    COMBO(combo_lt, KC_LT),
    COMBO(combo_dlr, KC_DLR),
    COMBO(combo_eql, KC_EQL),
    COMBO(combo_slsh, KC_SLSH),
    COMBO(combo_lcbr, KC_LCBR),
    COMBO(combo_hash, KC_HASH),
};

const key_override_t ko_comm_ques = ko_make_basic(MOD_MASK_SHIFT, KC_COMM, KC_QUES);
const key_override_t ko_dot_exlm = ko_make_basic(MOD_MASK_SHIFT, KC_DOT, KC_EXLM);
const key_override_t ko_rsqt_ndsh = ko_make_basic(MOD_MASK_SHIFT, RSQT, NDSH);
const key_override_t ko_perc_at = ko_make_basic(MOD_MASK_SHIFT, KC_PERC, KC_AT);
const key_override_t ko_dqt_quot = ko_make_basic(MOD_MASK_SHIFT, KC_DQT, KC_QUOT);
const key_override_t ko_coln_dot = ko_make_basic(MOD_MASK_SHIFT, KC_COLN, KC_DOT);
const key_override_t ko_astr_circ = ko_make_basic(MOD_MASK_SHIFT, KC_ASTR, KC_CIRC);
const key_override_t ko_exlm_ques = ko_make_basic(MOD_MASK_SHIFT, KC_EXLM, KC_QUES);
const key_override_t ko_scln_comm = ko_make_basic(MOD_MASK_SHIFT, KC_SCLN, KC_COMM);
const key_override_t ko_slsh_bsls = ko_make_basic(MOD_MASK_SHIFT, KC_SLSH, KC_BSLS);
const key_override_t ko_lprn_rprn = ko_make_basic(MOD_MASK_SHIFT, KC_LPRN, KC_RPRN);
const key_override_t ko_lcbr_rcbr = ko_make_basic(MOD_MASK_SHIFT, KC_LCBR, KC_RCBR);
const key_override_t ko_lbrc_rbrc = ko_make_basic(MOD_MASK_SHIFT, KC_LBRC, KC_RBRC);
const key_override_t ko_lt_gt = ko_make_basic(MOD_MASK_SHIFT, KC_LT, KC_GT);
const key_override_t ko_ldqt_rdqt = ko_make_basic(MOD_MASK_SHIFT, LDQT, RDQT);
const key_override_t ko_lsqt_rsqt = ko_make_basic(MOD_MASK_SHIFT, LSQT, RSQT);

// This globally defines all key overrides to be used
const key_override_t *key_overrides[] = {
    &ko_comm_ques,
    &ko_dot_exlm,
    &ko_rsqt_ndsh,
    &ko_perc_at,
    &ko_dqt_quot,
    &ko_coln_dot,
    &ko_astr_circ,
    &ko_exlm_ques,
    &ko_scln_comm,
    &ko_slsh_bsls,
    &ko_lprn_rprn,
    &ko_lcbr_rcbr,
    &ko_lbrc_rbrc,
    &ko_lt_gt,
    &ko_ldqt_rdqt,
    &ko_lsqt_rsqt,
};

// clang-format on

static const key_override_t *autoshift_override(uint16_t keycode) {
    for (uint8_t i = 0; i < ARRAY_SIZE(key_overrides); ++i) {
        if (key_overrides[i]->trigger == keycode) {
            return key_overrides[i];
        }
    }
    return NULL;
}
