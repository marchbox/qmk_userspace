// Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "users/holykeebs/holykeebs.h"
#include "dynamic_keymap.h"
#include "qmk_settings.h"
#include "vial.h"
#include "via.h"
#include <string.h>

#define LDQT RALT(KC_LBRC)
#define RDQT RSA(KC_LBRC)
#define LSQT RALT(KC_RBRC)
#define RSQT RSA(KC_RBRC)
#define NDSH RALT(KC_MINS)
#define HRML(K1, K2, K3, K4) LSFT_T(K1), LALT_T(K2), LCTL_T(K3), LGUI_T(K4)
#define HRMR(K1, K2, K3, K4) RGUI_T(K1), RCTL_T(K2), RALT_T(K3), RSFT_T(K4)
#define LAYOUT_wrapper(...) LAYOUT(__VA_ARGS__)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_wrapper(
        KC_Q, KC_W, KC_F, KC_P, KC_B,                      KC_J, KC_L, KC_U, KC_Y, KC_MINS,
        HRML(KC_A, KC_R, KC_S, KC_T), KC_G,                KC_M, HRMR(KC_N, KC_E, KC_I, KC_O),
        KC_Z, KC_X, KC_C, KC_D, KC_V,                      KC_K, KC_H, KC_COMM, KC_DOT, RSQT,
                                    TD(1), LT(1, KC_SPC), LT(2, KC_BSPC), OSM(MOD_RCTL)
    ),
    [1] = LAYOUT_wrapper(
        KC_1, KC_2, KC_3, KC_4, KC_5,                      KC_6, KC_7, KC_8, KC_9, KC_0,
        HRML(KC_MINS, KC_PLUS, KC_EQL, KC_DQT), KC_COLN,    KC_SCLN, HRMR(KC_LPRN, KC_RPRN, KC_LT, KC_GT),
        KC_ASTR, KC_DLR, KC_AMPR, KC_EXLM, KC_GRV,          KC_SLSH, KC_LCBR, KC_RCBR, KC_LBRC, KC_RBRC,
                                    KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS
    ),
    [2] = LAYOUT_wrapper(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5,                 KC_F6, KC_F7, KC_F8, KC_F9, KC_F10,
        LSFT_T(KC_F11), OSM(MOD_LALT), OSM(MOD_LCTL), OSM(MOD_LGUI), KC_NO,
                                                         KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_F12,
        G(KC_Z), G(KC_X), G(KC_C), G(KC_V), LSA(LGUI(KC_V)), KC_PGDN, KC_END, KC_HOME, KC_PGUP, KC_NO,
                                    KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS
    ),
    [3] = LAYOUT_wrapper(
        HK_I_SCROLL, KC_NO, KC_NO, KC_NO, KC_NO,           KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        HK_D_MODE_T, HK_D_MODE, MS_BTN2, MS_BTN1, HK_S_MODE,
                                                         QK_LAYER_LOCK, RGUI_T(KC_VOLD), RCTL_T(KC_MUTE), RALT_T(KC_VOLU), KC_RSFT,
        HK_SAVE, KC_NO, KC_NO, KC_NO, KC_NO,               KC_NO, KC_MPRV, KC_MPLY, KC_MNXT, KC_NO,
                                    KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS
    ),
};

static const vial_tap_dance_entry_t default_tap_dances[] = {
    {LCTL(LSG(KC_4)), KC_NO, LSG(KC_5), KC_NO, 230},
    {OSM(MOD_LSFT), MO(3), KC_NO, KC_NO, 150},
};

static const vial_combo_entry_t default_combos[] = {
    {{KC_W, KC_F}, KC_ESC},
    {{KC_2, KC_3}, KC_ESC},
    {{LALT_T(KC_R), LCTL_T(KC_S)}, KC_TAB},
    {{LALT_T(KC_PLUS), LCTL_T(KC_EQL)}, KC_TAB},
    {{KC_X, KC_C}, CW_TOGG},
    {{KC_DLR, KC_AMPR}, CW_TOGG},
    {{KC_U, KC_Y}, KC_DEL},
    {{KC_8, KC_9}, KC_DEL},
    {{RCTL_T(KC_E), RALT_T(KC_I)}, KC_BSPC},
    {{RCTL_T(KC_RPRN), RALT_T(KC_LT)}, KC_BSPC},
    {{KC_COMM, KC_DOT}, KC_ENT},
    {{KC_RCBR, KC_LBRC}, KC_ENT},
    {{KC_Z, KC_X, KC_C, KC_D}, G(KC_SPC)},
    {{KC_H, KC_COMM, KC_DOT, RSQT}, KC_MUTE},
    {{KC_X, KC_C, KC_D}, KC_SPC},
    {{KC_DLR, KC_AMPR, KC_EXLM}, KC_SPC},
    {{KC_H, KC_COMM, KC_DOT}, KC_SPC},
    {{KC_LCBR, KC_RCBR, KC_LBRC}, KC_SPC},
    {{KC_Q, KC_W, KC_F, KC_P}, QK_MACRO_0},
    {{KC_L, KC_U, KC_Y, KC_MINS}, TD(0)},
};

#define SHIFT_OVERRIDE(trigger_, replacement_) \
    { .trigger = (trigger_), .replacement = (replacement_), .layers = 0x0003, \
      .trigger_mods = MOD_MASK_SHIFT, .suppressed_mods = MOD_MASK_SHIFT, \
      .options = vial_ko_enabled | vial_ko_option_activation_trigger_down | \
                 vial_ko_option_activation_required_mod_down | vial_ko_option_activation_negative_mod_up }

static const vial_key_override_entry_t default_overrides[] = {
    SHIFT_OVERRIDE(KC_COMM, KC_QUES),
    SHIFT_OVERRIDE(KC_DOT, KC_EXLM),
    SHIFT_OVERRIDE(RSQT, NDSH),
    SHIFT_OVERRIDE(KC_EQL, KC_PERC),
    SHIFT_OVERRIDE(KC_DQT, KC_QUOT),
    SHIFT_OVERRIDE(KC_COLN, KC_DOT),
    SHIFT_OVERRIDE(KC_ASTR, KC_CIRC),
    SHIFT_OVERRIDE(KC_DLR, KC_HASH),
    SHIFT_OVERRIDE(KC_AMPR, KC_PIPE),
    SHIFT_OVERRIDE(KC_EXLM, KC_QUES),
    SHIFT_OVERRIDE(KC_SCLN, KC_COMM),
    SHIFT_OVERRIDE(KC_RPRN, KC_AT),
    SHIFT_OVERRIDE(KC_SLSH, KC_BSLS),
    SHIFT_OVERRIDE(KC_LCBR, LDQT),
    SHIFT_OVERRIDE(KC_RCBR, RDQT),
    SHIFT_OVERRIDE(KC_LBRC, LSQT),
    SHIFT_OVERRIDE(KC_RBRC, RSQT),
};

typedef struct {
    uint16_t keycode;
    uint16_t tap;
    uint16_t shifted;
} tap_hold_override_t;

static const tap_hold_override_t tap_hold_overrides[] = {
    {LALT_T(KC_PLUS), KC_PLUS, 0},
    {LCTL_T(KC_EQL), KC_EQL, KC_PERC},
    {LGUI_T(KC_DQT), KC_DQT, KC_QUOT},
    {RGUI_T(KC_LPRN), KC_LPRN, 0},
    {RCTL_T(KC_RPRN), KC_RPRN, KC_AT},
    {RALT_T(KC_LT), KC_LT, 0},
    {RSFT_T(KC_GT), KC_GT, 0},
};

static const struct {
    uint16_t id;
    uint32_t value;
} default_settings[] = {
    {1, 0}, {2, 50}, {3, 0}, {4, 175}, {5, 5}, {6, 5000}, {7, 175},
    {9, 10}, {10, 20}, {11, 4}, {12, 10}, {13, 30}, {14, 10}, {15, 80},
    {16, 8}, {17, 80}, {18, 0}, {19, 80}, {20, 5}, {21, 0}, {22, 1},
    {23, 0}, {24, 0}, {25, 0}, {26, 1}, {27, 150},
};
// clang-format on

// Vial macro delays use nonzero base-255 bytes, unlike QMK's SS_DELAY string.
static const uint8_t default_macro[] = {
    SS_QMK_PREFIX, SS_DOWN_CODE, KC_LGUI,
    SS_QMK_PREFIX, SS_DOWN_CODE, KC_LCTL,
    SS_QMK_PREFIX, SS_DOWN_CODE, KC_Q,
    SS_QMK_PREFIX, SS_UP_CODE, KC_LGUI,
    SS_QMK_PREFIX, SS_UP_CODE, KC_LCTL,
    SS_QMK_PREFIX, SS_UP_CODE, KC_Q,
    SS_QMK_PREFIX, SS_DELAY_CODE, (1000 % 255) + 1, (1000 / 255) + 1,
    SS_QMK_PREFIX, SS_TAP_CODE, KC_ESC,
    0,
};

_Static_assert(ARRAY_SIZE(default_tap_dances) <= VIAL_TAP_DANCE_ENTRIES, "Not enough Vial tap dance slots");
_Static_assert(ARRAY_SIZE(default_combos) <= VIAL_COMBO_ENTRIES, "Not enough Vial combo slots");
_Static_assert(ARRAY_SIZE(default_overrides) <= VIAL_KEY_OVERRIDE_ENTRIES, "Not enough Vial override slots");

static void restore_vial_defaults(void) {
    via_eeprom_set_valid(false);
    for (uint8_t i = 0; i < ARRAY_SIZE(default_tap_dances); ++i) {
        dynamic_keymap_set_tap_dance(i, &default_tap_dances[i]);
    }
    for (uint8_t i = 0; i < ARRAY_SIZE(default_combos); ++i) {
        dynamic_keymap_set_combo(i, &default_combos[i]);
    }
    for (uint8_t i = 0; i < ARRAY_SIZE(default_overrides); ++i) {
        dynamic_keymap_set_key_override(i, &default_overrides[i]);
    }
    for (uint8_t i = 0; i < ARRAY_SIZE(default_settings); ++i) {
        qmk_settings_set(default_settings[i].id, &default_settings[i].value, sizeof(default_settings[i].value));
    }
    uint8_t macro[sizeof(default_macro)];
    memcpy(macro, default_macro, sizeof(macro));
    dynamic_keymap_macro_set_buffer(0, sizeof(macro), macro);
    vial_init();
    via_eeprom_set_valid(true);
}

void via_init_kb(void) {
    if (!via_eeprom_is_valid()) {
        eeconfig_init_via();
        restore_vial_defaults();
    }
}

void eeconfig_init_keymap(void) {
    restore_vial_defaults();
}

bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    for (uint8_t i = 0; i < ARRAY_SIZE(tap_hold_overrides); ++i) {
        if (keycode != tap_hold_overrides[i].keycode) {
            continue;
        }
        if (record->tap.count && record->event.pressed) {
            uint16_t shifted = tap_hold_overrides[i].shifted;
            if (shifted && ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT)) {
                uint8_t mods    = get_mods();
                uint8_t oneshot = get_oneshot_mods();
                del_mods(MOD_MASK_SHIFT);
                del_oneshot_mods(MOD_MASK_SHIFT);
                tap_code16(shifted);
                set_mods(mods);
                set_oneshot_mods(oneshot);
            } else {
                tap_code16(tap_hold_overrides[i].tap);
            }
            return false;
        }
        break;
    }
    return true;
}
