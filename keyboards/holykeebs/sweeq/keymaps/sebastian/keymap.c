// Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "users/holykeebs/holykeebs.h"
#include "users/holykeebs/pointing.h"
#include "dynamic_keymap.h"
#include "qmk_settings.h"
#include "vial.h"
#include "via.h"
#include "pointing_device_auto_mouse.h"
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
        OSM(MOD_LSFT), LT(1, KC_SPC),
        LT(2, KC_BSPC), QK_REPEAT_KEY
    ),
    [1] = LAYOUT_wrapper(
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
        KC_TRNS, KC_TRNS,
        MO(3), KC_TRNS
    ),
    [2] = LAYOUT_wrapper(
        // row 1
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        // row 2
        OSM(MOD_LSFT), OSM(MOD_RALT), OSM(MOD_LCTL), OSM(MOD_LGUI), KC_NO,
        KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_NO,
        // row 3
        G(KC_Z), G(KC_X), G(KC_C), G(KC_V), LSA(LGUI(KC_V)),
        KC_PGDN, KC_END, KC_HOME, KC_PGUP, KC_NO,
        // thumbs
        KC_TRNS, MO(3),
        KC_TRNS, KC_TRNS
    ),
    [3] = LAYOUT_wrapper(
        // row 1
        HK_I_SCROLL, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        // row 2
        HK_D_MODE_T, MS_BTN2, HK_D_MODE, MS_BTN1, HK_S_MODE,
        HK_D_MODE, KC_RGUI, KC_RCTL, KC_RALT, KC_RSFT,
        // row 3
        HK_SAVE, KC_NO, KC_NO, KC_NO, QK_LAYER_LOCK,
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        // thumbs
        KC_TRNS, KC_TRNS,
        MS_BTN1, MS_BTN2
    ),
    [4] = LAYOUT_wrapper(
        // row 1
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5,
        KC_F6, KC_F7, KC_F8, KC_F9, KC_F10,
        // row 2
        KC_LSFT, KC_LALT, KC_LCTL, KC_LGUI, KC_F11,
        KC_F12, RGUI_T(KC_VOLD), RCTL_T(KC_MUTE), RALT_T(KC_VOLU), KC_RSFT,
        // row 3
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_MPRV, KC_MPLY, KC_MNXT, KC_NO,
        // thumbs
        KC_NO, KC_NO,
        KC_NO, KC_NO
    ),
};

static const vial_tap_dance_entry_t default_tap_dances[] = {
    {LCTL(LSG(KC_4)), KC_NO, LSG(KC_5), KC_NO, 230},
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
    {{KC_F1, KC_F2, KC_F3, KC_F4}, QK_BOOT},
    {{OSM(MOD_LSFT), QK_REPEAT_KEY}, MO(4)},
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
} tap_hold_override_t;

static const tap_hold_override_t tap_hold_overrides[] = {
    {LALT_T(KC_PLUS), KC_PLUS},
    {LCTL_T(KC_EQL), KC_EQL},
    {LGUI_T(KC_DQT), KC_DQT},
    {RGUI_T(KC_LPRN), KC_LPRN},
    {RCTL_T(KC_RPRN), KC_RPRN},
    {RALT_T(KC_LT), KC_LT},
    {RSFT_T(KC_GT), KC_GT},
};

// clang-format on
static uint16_t autoshift_tap_keycode(uint16_t keycode) {
    for (uint8_t i = 0; i < ARRAY_SIZE(tap_hold_overrides); ++i) {
        if (keycode == tap_hold_overrides[i].keycode) {
            return tap_hold_overrides[i].tap;
        }
    }
    return get_tap_keycode(keycode);
}

static const vial_key_override_entry_t *autoshift_override(uint16_t keycode, keyrecord_t *record) {
    uint8_t layer = read_source_layers_cache(record->event.key);
    for (uint8_t i = 0; i < ARRAY_SIZE(default_overrides); ++i) {
        const vial_key_override_entry_t *override = &default_overrides[i];
        if (override->trigger == keycode && (override->layers & (1UL << layer))) {
            return override;
        }
    }
    return NULL;
}

bool vial_get_auto_shifted_key(uint16_t keycode, keyrecord_t *record);

bool get_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
    if (!QS_auto_shift_enable) {
        return false;
    }
    uint16_t tap_keycode = autoshift_tap_keycode(keycode);
    bool has_override = autoshift_override(tap_keycode, record) != NULL;
    if (IS_QK_MOD_TAP(keycode) && !(tap_keycode >= KC_A && tap_keycode <= KC_Z) && !has_override) {
        return false;
    }
    if (has_override && !QS_auto_shift_no_auto_shift_special) {
        return true;
    }
    return vial_get_auto_shifted_key(tap_keycode, record);
}

void autoshift_press_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    keycode = autoshift_tap_keycode(keycode);
    const vial_key_override_entry_t *override = shifted ? autoshift_override(keycode, record) : NULL;
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
    keycode = autoshift_tap_keycode(keycode);
    const vial_key_override_entry_t *override = shifted ? autoshift_override(keycode, record) : NULL;
    unregister_code16(override ? override->replacement : keycode);
}

// clang-format off
static const struct {
    uint16_t id;
    uint32_t value;
} default_settings[] = {
    {1, 0}, {2, 50}, {3, 1}, {4, 175}, {5, 5}, {6, 5000}, {7, 175},
    {9, 10}, {10, 20}, {11, 4}, {12, 10}, {13, 30}, {14, 10}, {15, 80},
    {16, 8}, {17, 80}, {18, 0}, {19, 80}, {20, 5}, {21, 0}, {22, 1},
    {23, 0}, {24, 1}, {25, 0}, {26, 1}, {27, 150},
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

bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    (void)keycode;
    (void)record;
    if (combo_index == ARRAY_SIZE(default_combos) - 1 && combo->keycode == MO(4)) {
        return get_highest_layer(layer_state | default_layer_state) == 0;
    }
    return true;
}

bool process_combo_key_release(uint16_t combo_index, combo_t *combo, uint8_t key_index, uint16_t keycode) {
    (void)key_index;
    (void)keycode;
    return combo_index == ARRAY_SIZE(default_combos) - 1 && combo->keycode == MO(4);
}

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
    } else if (dynamic_keymap_get_keycode(0, 3, 3) == TD(1)) {
        dynamic_keymap_set_keycode(0, 3, 3, LT(3, KC_LSFT));
    }
}

void eeconfig_init_keymap(void) {
    restore_vial_defaults();
}

void keyboard_post_init_keymap(void) {
    g_hk_state.main.scroll_direction_inverted = true;
    set_auto_mouse_enable(true);
    // Older builds saved a zero timeout because auto mouse was not compiled in.
    if (!get_auto_mouse_timeout()) {
        set_auto_mouse_timeout(AUTO_MOUSE_TIME);
    }
}

bool is_mouse_record_user(uint16_t keycode, keyrecord_t *record) {
    (void)record;
    switch (keycode) {
        case LT(3, KC_LSFT):
        case HK_D_MODE:
        case HK_D_MODE_T:
        case HK_S_MODE:
        case HK_S_MODE_T:
        case HK_I_SCROLL:
        case HK_C_SCROLL:
            return true;
        default:
            return false;
    }
}

bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    // LT() cannot encode OSM() in its tap keycode.
    if (keycode == LT(3, KC_LSFT) && record->tap.count) {
        if (record->event.pressed) {
            set_oneshot_mods(get_oneshot_mods() | MOD_LSFT);
        }
        return false;
    }

    for (uint8_t i = 0; i < ARRAY_SIZE(tap_hold_overrides); ++i) {
        if (keycode != tap_hold_overrides[i].keycode) {
            continue;
        }
        if (get_retro_tapping(keycode, record) && get_auto_shifted_key(keycode, record) &&
            !((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT)) {
            return true;
        }
        if (record->tap.count && record->event.pressed) {
            const vial_key_override_entry_t *override = autoshift_override(tap_hold_overrides[i].tap, record);
            uint16_t shifted = override ? override->replacement : 0;
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
