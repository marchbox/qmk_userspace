"""Test the keymap hooks with QMK headers: python3 test_symbols.py QMK_ROOT."""

from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[2]
FIRMWARE = Path(sys.argv.pop(1)).resolve()
KEYMAP = ROOT / "keyboards/holykeebs/sweeq/keymaps/sebastian"
SOURCE = (KEYMAP / "keymap.c").read_text()


def section(start, end):
    offset = SOURCE.index(start)
    return SOURCE[offset:SOURCE.index(end, offset)]


class SymbolTests(unittest.TestCase):
    def test_positions_combos_and_shift(self):
        harness = r"""
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "quantum_keycodes.h"
#include "modifiers.h"
#define PROGMEM
#define MATRIX_ROWS 1
#define MATRIX_COLS 34
#define LAYOUT(...) {{__VA_ARGS__}}
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define MOD_BIT(k) (1U << ((k) - KC_LCTL))
#define IS_COMBOEVENT(e) ((e).combo)
#define VIAL_KEY_OVERRIDE_ENTRIES 32
enum { HK_I_SCROLL = QK_USER, HK_D_MODE_T, HK_D_MODE, HK_S_MODE, HK_SAVE };
typedef struct { uint8_t row, col; } keypos_t;
typedef struct {
    struct { keypos_t key; bool pressed, combo; } event;
    struct { uint8_t count; } tap;
} keyrecord_t;
typedef struct { uint16_t input[4], output; } vial_combo_entry_t;
typedef struct {
    uint16_t trigger, replacement, layers;
    uint8_t trigger_mods, negative_mod_mask, suppressed_mods, options;
} vial_key_override_entry_t;
typedef uint32_t layer_state_t;
typedef struct {
    uint16_t trigger, replacement;
    layer_state_t layers;
    uint8_t suppressed_mods;
    bool (*custom_action)(bool, void *);
    void *context;
} key_override_t;
enum {
    vial_ko_option_activation_trigger_down = 1,
    vial_ko_option_activation_required_mod_down = 2,
    vial_ko_option_activation_negative_mod_up = 4,
    vial_ko_enabled = 128
};
static uint8_t cached_layer, mods, weak_mods, oneshot_mods, last_mods;
static uint16_t output, released, last_keycode;
static int repeat_count;
static bool QS_auto_shift_enable = true, QS_auto_shift_no_auto_shift_special;
static bool retro_tapping;
static uint8_t read_source_layers_cache(keypos_t key) { return cached_layer; }
static uint16_t get_tap_keycode(uint16_t keycode) {
    return IS_QK_MOD_TAP(keycode) ? QK_MOD_TAP_GET_TAP_KEYCODE(keycode) : keycode;
}
static int get_repeat_key_count(void) { return repeat_count; }
static uint16_t get_last_keycode(void) { return last_keycode; }
static uint8_t get_last_mods(void) { return last_mods; }
static void set_last_keycode(uint16_t keycode) { last_keycode = keycode; }
static void set_last_mods(uint8_t value) { last_mods = value; }
static uint8_t get_mods(void) { return mods; }
static uint8_t get_weak_mods(void) { return weak_mods; }
static uint8_t get_oneshot_mods(void) { return oneshot_mods; }
static void del_mods(uint8_t value) { mods &= ~value; }
static void del_weak_mods(uint8_t value) { weak_mods &= ~value; }
static void del_oneshot_mods(uint8_t value) { oneshot_mods &= ~value; }
static void add_weak_mods(uint8_t value) { weak_mods |= value; }
static void set_mods(uint8_t value) { mods = value; }
static void set_oneshot_mods(uint8_t value) { oneshot_mods = value; }
static void register_code16(uint16_t keycode) { output = keycode; }
static void unregister_code16(uint16_t keycode) { released = keycode; }
static void tap_code16(uint16_t keycode) { output = keycode; released = keycode; }
static bool get_retro_tapping(uint16_t keycode, keyrecord_t *record) { return retro_tapping; }
bool vial_get_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
    return (keycode >= KC_1 && keycode <= KC_0) || keycode == KC_GRV ||
           keycode == KC_EQL || keycode == KC_MINS || keycode == KC_BSLS;
}
"""
        harness += section("#define LDQT", "static const vial_tap_dance_entry_t")
        harness += section("static const vial_combo_entry_t", "// clang-format on")
        harness += section("static uint16_t autoshift_tap_keycode", "// clang-format off\nstatic const struct")
        harness += SOURCE[SOURCE.index("bool process_record_keymap"):]
        slots = re.search(r"#define VIAL_COMBO_ENTRIES (\d+)", (KEYMAP / "config.h").read_text())
        self.assertIsNotNone(slots, "The new combos need more than Vial's default 32 slots")
        harness += f'\n_Static_assert(ARRAY_SIZE(default_combos) <= {slots[1]}, "Combo capacity");\n'
        harness += r"""
const key_override_t *vial_key_override_get(uint16_t index) {
    if (index >= ARRAY_SIZE(default_overrides)) return NULL;
    static key_override_t override;
    const vial_key_override_entry_t *entry = &default_overrides[index];
    override = (key_override_t){
        .trigger = entry->trigger, .replacement = entry->replacement,
        .layers = entry->layers, .suppressed_mods = entry->suppressed_mods
    };
    return &override;
}
static void check_combo(uint8_t layer, uint8_t first, uint8_t second, uint16_t result) {
    size_t matches = 0;
    for (size_t i = 0; i < ARRAY_SIZE(default_combos); ++i) {
        const vial_combo_entry_t *combo = &default_combos[i];
        if (combo->input[0] == keymaps[layer][0][first] &&
            combo->input[1] == keymaps[layer][0][second] && !combo->input[2]) {
            assert(combo->output == result);
            ++matches;
        }
    }
    assert(matches == 1);
}
static void check_symbol(uint16_t keycode, uint16_t plain, uint16_t shifted, bool combo) {
    keyrecord_t record = {.event = {.pressed = true, .combo = combo}, .tap = {.count = 1}};
    cached_layer = combo ? 4 : 1;
    mods = oneshot_mods = weak_mods = 0;
    last_keycode = keycode;
    last_mods = 0;
    assert(get_auto_shifted_key(keycode, &record));
    autoshift_press_user(keycode, false, &record);
    assert(output == plain);
    autoshift_release_user(keycode, false, &record);
    assert(released == plain);
    last_keycode = keycode;
    autoshift_press_user(keycode, true, &record);
    const vial_key_override_entry_t *override = autoshift_override(plain, &record);
    if (override) {
        assert(output == shifted);
        assert(last_keycode == shifted && !(last_mods & MOD_MASK_SHIFT));
        assert(override->trigger_mods == MOD_MASK_SHIFT);
        assert(override->suppressed_mods == MOD_MASK_SHIFT);
    } else {
        assert(output == plain && (weak_mods & MOD_LSFT));
        assert(shifted == plain || shifted == LSFT(plain));
    }
    autoshift_release_user(keycode, true, &record);
    assert(released == (override ? shifted : plain));
    if (combo && override) {
        assert(process_record_keymap(keycode, &record));
        size_t index = override - default_overrides;
        const key_override_t *runtime = key_override_get(index);
        assert(runtime->layers & (1UL << cached_layer));
        assert(runtime->replacement == shifted);
        mods = MOD_BIT(KC_RSFT);
        assert(runtime->custom_action(true, runtime->context));
        assert(last_keycode == shifted && !(last_mods & MOD_MASK_SHIFT));
        record.event.combo = false;
        assert(process_record_keymap(KC_Q, &record));
        assert(key_override_get(index)->layers == override->layers);
    }
    if (IS_QK_MOD_TAP(keycode)) {
        for (int shift = 0; shift < 3; ++shift) {
            mods = shift == 0 ? MOD_LSFT : shift == 1 ? MOD_BIT(KC_RSFT) : 0;
            oneshot_mods = shift == 2 ? MOD_LSFT : 0;
            uint8_t saved_mods = mods, saved_oneshot = oneshot_mods;
            assert(!process_record_keymap(keycode, &record));
            assert(output == (override ? shifted : plain));
            assert(mods == saved_mods && oneshot_mods == saved_oneshot);
        }
        mods = oneshot_mods = 0;
        assert(!process_record_keymap(keycode, &record));
        assert(output == plain);
        retro_tapping = true;
        assert(process_record_keymap(keycode, &record));
        retro_tapping = false;
        record.tap.count = 0;
        assert(process_record_keymap(keycode, &record));
    }
}
int main(void) {
    const uint8_t positions[][2] = {
        {0, 1}, {2, 3}, {6, 7}, {8, 9}, {10, 11}, {12, 13},
        {16, 17}, {18, 19}, {20, 21}, {22, 23}, {26, 27}, {28, 29}
    };
    const uint16_t symbols[] = {
        KC_AT, KC_COLN, KC_SCLN, KC_SLSH, KC_LBRC, KC_DQT,
        KC_LPRN, KC_LT, KC_DLR, KC_EQL, KC_LCBR, KC_HASH
    };
    const uint16_t shifted[] = {
        KC_AT, KC_DOT, KC_COMM, KC_BSLS, KC_RBRC, KC_QUOT,
        KC_RPRN, KC_GT, KC_DLR, KC_PLUS, KC_RCBR, KC_HASH
    };
    for (size_t i = 0; i < ARRAY_SIZE(symbols); ++i) {
        check_combo(0, positions[i][0], positions[i][1], symbols[i]);
        check_symbol(symbols[i], symbols[i], shifted[i], true);
    }
    check_combo(0, 1, 3, KC_END);
    check_combo(0, 6, 8, KC_HOME);
    check_combo(0, 11, 13, KC_RGHT);
    check_combo(0, 16, 18, KC_DOWN);
    check_combo(0, 21, 23, KC_LEFT);
    check_combo(0, 26, 28, KC_UP);
    const uint8_t layer1_positions[] = {10, 11, 16, 17, 18, 19, 21, 22, 26, 27, 28, 29};
    const uint16_t layer1_symbols[] = {
        KC_PERC, KC_MINS, KC_LPRN, KC_LCBR, KC_LBRC, KC_LT,
        KC_PIPE, KC_AMPR, KC_DLR, KC_HASH, LDQT, LSQT
    };
    const uint16_t layer1_shifted[] = {
        KC_AT, KC_UNDS, KC_RPRN, KC_RCBR, KC_RBRC, KC_GT,
        KC_PIPE, KC_AMPR, KC_DLR, KC_HASH, RDQT, RSQT
    };
    for (size_t i = 0; i < ARRAY_SIZE(layer1_positions); ++i)
        check_symbol(keymaps[1][0][layer1_positions[i]], layer1_symbols[i], layer1_shifted[i], false);
    check_combo(1, 11, 12, KC_TAB);
    check_combo(1, 21, 22, CW_TOGG);
    check_combo(1, 17, 18, KC_BSPC);
    check_combo(1, 27, 28, KC_ENT);
    for (size_t i = 0; i < ARRAY_SIZE(default_combos); ++i) {
        for (size_t j = 0; j < ARRAY_SIZE(default_combos[i].input); ++j) {
            uint16_t input = default_combos[i].input[j];
            if (!input) continue;
            bool present = false;
            for (size_t layer = 0; layer < ARRAY_SIZE(keymaps); ++layer)
                for (size_t key = 0; key < MATRIX_COLS; ++key)
                    present |= keymaps[layer][0][key] == input;
            assert(present);
        }
    }
    keyrecord_t record = {0};
    cached_layer = 1;
    assert(!autoshift_override(KC_DLR, &record));
    assert(!autoshift_override(KC_AMPR, &record));
    assert(!autoshift_override(KC_RCBR, &record));
    assert(!autoshift_override(KC_RBRC, &record));
    repeat_count = 1;
    assert(!get_auto_shifted_key(KC_LPRN, &record));
    assert(!key_override_get(0));
    repeat_count = 0;
    QS_auto_shift_enable = false;
    assert(!get_auto_shifted_key(KC_LPRN, &record));
    return 0;
}
"""
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "symbols.c"
            binary = Path(directory) / "symbols"
            source.write_text(harness)
            subprocess.run(
                ["clang", "-std=c11", "-Wall", "-Werror", "-Wno-unused-function",
                 "-I", str(FIRMWARE / "quantum"),
                 "-I", str(FIRMWARE / "quantum/keymap_extras"),
                 "-I", str(FIRMWARE / "quantum/sequencer"),
                 str(source), "-o", str(binary)],
                check=True,
            )
            subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    unittest.main()
