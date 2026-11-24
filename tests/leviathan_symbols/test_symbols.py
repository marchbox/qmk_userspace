"""Test the native QMK keymap hooks: python3 test_symbols.py QMK_ROOT."""

from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[2]
FIRMWARE = Path(sys.argv.pop(1)).resolve()
SOURCE = (
    ROOT / "keyboards/bastardkb/charybdis/3x5/keymaps/leviathan/keymap.c"
).read_text()


def section(start, end):
    offset = SOURCE.index(start)
    return SOURCE[offset:SOURCE.index(end, offset)]


class SymbolTests(unittest.TestCase):
    def test_combos_autoshift_and_home_row_mods(self):
        harness = r"""
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "quantum_keycodes.h"
#include "modifiers.h"
#define PROGMEM
#define MATRIX_ROWS 1
#define MATRIX_COLS 35
#define LAYOUT(...) {{__VA_ARGS__}}
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define MOD_BIT(k) (1U << ((k) - KC_LCTL))
#define COMBO_END 0
#define COMBO(keys, output) {.input = keys, .keycode = output}
typedef struct {
    struct { bool pressed; uint16_t time; } event;
    struct { uint8_t count; bool interrupted; } tap;
} keyrecord_t;
typedef typeof(((keyrecord_t *)0)->event) keyevent_t;
typedef struct { const uint16_t *input; uint16_t keycode; } combo_t;
typedef struct { uint16_t trigger, replacement; } key_override_t;
#define ko_make_basic(mods, key, value) { .trigger = key, .replacement = value }
enum { DRGSCRL = QK_USER + 10 };
static uint8_t mods, weak_mods, oneshot_mods;
static uint16_t output, released;
static uint16_t now;
static uint8_t get_mods(void) { return mods; }
static uint8_t get_oneshot_mods(void) { return oneshot_mods; }
static void del_mods(uint8_t value) { mods &= ~value; }
static void add_mods(uint8_t value) { mods |= value; }
static void del_weak_mods(uint8_t value) { weak_mods &= ~value; }
static void del_oneshot_mods(uint8_t value) { oneshot_mods &= ~value; }
static void add_weak_mods(uint8_t value) { weak_mods |= value; }
static void set_mods(uint8_t value) { mods = value; }
static void set_oneshot_mods(uint8_t value) { oneshot_mods = value; }
static void register_code16(uint16_t keycode) { output = keycode; }
static void unregister_code16(uint16_t keycode) { released = keycode; }
static void tap_code16(uint16_t keycode) { output = released = keycode; }
static void tap_code(uint16_t keycode) { tap_code16(keycode); }
static void wait_ms(uint16_t delay) {}
static uint16_t timer_read(void) { return now; }
static void send_keyboard_report(void) {}
static void clear_oneshot_layer_state(uint8_t state) {}
bool get_autoshift_state(void);
#define AUTO_SHIFT_TIMEOUT 175
#define RETRO_SHIFT 500
#define AUTO_SHIFT_NO_SETUP
#define TAP_CODE_DELAY 0
#define TIMER_DIFF_16(a, b) ((uint16_t)((a) - (b)))
#define ONESHOT_OTHER_KEY_PRESSED 1
#define IS_RETRO(kc) (IS_QK_MOD_TAP(kc) || IS_QK_LAYER_TAP(kc))
"""
        harness += section("enum charybdis_keymap_layers", "// Tap dance:")
        harness += section("typedef struct {", "#define LAYOUT_wrapper")
        harness += SOURCE[SOURCE.index("#define LAYOUT_wrapper"):]
        engine = (
            FIRMWARE / "quantum/process_keycode/process_auto_shift.c"
        ).read_text()
        engine = re.sub(r"^#include .*\n", "", engine, flags=re.MULTILINE)
        for hook in (
            "get_auto_shifted_key", "autoshift_press_user", "autoshift_release_user"
        ):
            engine, count = re.subn(
                rf"__attribute__\(\(weak\)\) (?:bool|void) {hook}\(.*?^\}}",
                "",
                engine,
                flags=re.MULTILINE | re.DOTALL,
            )
            self.assertEqual(count, 1)
        harness += engine
        harness += r"""
static void check_combo(uint8_t layer, uint8_t first, uint8_t second, uint16_t result) {
    size_t matches = 0;
    for (size_t i = 0; i < ARRAY_SIZE(key_combos); ++i) {
        const combo_t *combo = &key_combos[i];
        if (combo->input[0] == keymaps[layer][0][first] &&
            combo->input[1] == keymaps[layer][0][second] && !combo->input[2]) {
            assert(combo->keycode == result);
            ++matches;
        }
    }
    assert(matches == 1);
}
static void check_symbol(uint16_t keycode, uint16_t plain, uint16_t shifted) {
    keyrecord_t record = {.event = {.pressed = true}, .tap = {.count = 1}};
    mods = oneshot_mods = weak_mods = 0;
    assert(get_auto_shifted_key(keycode, &record));
    autoshift_press_user(keycode, false, &record);
    assert(output == plain);
    autoshift_release_user(keycode, false, &record);
    assert(released == plain);
    autoshift_press_user(keycode, true, &record);
    const key_override_t *override = autoshift_override(plain);
    if (override) {
        assert(output == shifted && !(weak_mods & MOD_MASK_SHIFT));
    } else {
        assert(output == plain && (weak_mods & MOD_LSFT));
        assert(shifted == plain || shifted == LSFT(plain));
    }
    autoshift_release_user(keycode, true, &record);
    assert(released == (override ? shifted : plain));
    bool manual_tap = false;
    for (size_t i = 0; i < ARRAY_SIZE(tap_hold_overrides); ++i)
        manual_tap |= keycode == tap_hold_overrides[i].tap_hold_keycode;
    if (manual_tap) {
        weak_mods = 0;
        assert(process_record_user(keycode, &record));
        mods = MOD_LSFT;
        record.event.pressed = false;
        assert(process_record_user(keycode, &record));
        record.event.pressed = true;
        for (int shift = 0; shift < 3; ++shift) {
            mods = shift == 0 ? MOD_LSFT : shift == 1 ? MOD_BIT(KC_RSFT) : 0;
            oneshot_mods = shift == 2 ? MOD_LSFT : 0;
            uint8_t saved_mods = mods, saved_oneshot = oneshot_mods;
            assert(!process_record_user(keycode, &record));
            assert(output == (override ? shifted : plain));
            assert(mods == saved_mods && oneshot_mods == saved_oneshot);
            record.event.pressed = false;
            mods = oneshot_mods = 0;
            released = KC_NO;
            assert(!process_record_user(keycode, &record));
            assert(released == KC_NO);
            record.event.pressed = true;
        }
        mods = oneshot_mods = 0;
        autoshift_disable();
        assert(!get_auto_shifted_key(keycode, &record));
        assert(!process_record_user(keycode, &record));
        assert(output == plain);
        record.event.pressed = false;
        assert(!process_record_user(keycode, &record));
        record.event.pressed = true;
        autoshift_enable();
        record.tap.count = 0;
        assert(process_record_user(keycode, &record));
    }
}
static void check_timed_symbol(uint16_t keycode, uint16_t plain, uint16_t shifted) {
    for (int shift = 0; shift < 2; ++shift) {
        keyrecord_t record = {.event = {.pressed = true}, .tap = {.count = 1}};
        if (!IS_RETRO(keycode)) record.tap.count = 0;
        mods = oneshot_mods = weak_mods = 0;
        output = released = KC_NO;
        now += 1000;
        retroshift_poll_time(&record.event);
        assert(process_record_user(keycode, &record));
        assert(!process_auto_shift(keycode, &record));
        assert(output == KC_NO);
        now += shift ? 200 : 100;
        record.event.pressed = false;
        assert(process_record_user(keycode, &record));
        assert(!process_auto_shift(keycode, &record));
        const key_override_t *override = autoshift_override(plain);
        assert(output == (shift && override ? shifted : plain));
        assert(released == output && !(weak_mods & MOD_MASK_SHIFT));
    }
    keyrecord_t hold = {.event = {.pressed = true}, .tap = {.count = 0}};
    assert(process_record_user(keycode, &hold));
    if (IS_RETRO(keycode)) {
        assert(process_auto_shift(keycode, &hold));
        hold.event.pressed = false;
        assert(process_auto_shift(keycode, &hold));
    }
}
int main(void) {
    const uint8_t positions[][2] = {
        {0, 1}, {2, 3}, {5, 6}, {6, 7}, {8, 9}, {10, 11}, {12, 13},
        {15, 16}, {16, 17}, {18, 19}, {20, 21}, {22, 23}, {25, 26}, {26, 27}, {28, 29}
    };
    const uint16_t symbols[] = {
        KC_AMPR, KC_COLN, KC_ASTR, KC_SCLN, KC_PIPE, KC_PERC, KC_DQT,
        KC_LBRC, KC_LPRN, KC_LT, KC_DLR, KC_EQL, KC_SLSH, KC_LCBR, KC_HASH
    };
    const uint16_t shifted[] = {
        KC_AMPR, KC_DOT, KC_CIRC, KC_COMM, KC_PIPE, KC_AT, KC_QUOT,
        KC_RBRC, KC_RPRN, KC_GT, KC_DLR, KC_PLUS, KC_BSLS, KC_RCBR, KC_HASH
    };
    for (size_t i = 0; i < ARRAY_SIZE(symbols); ++i) {
        check_combo(LAYER_DEF, positions[i][0], positions[i][1], symbols[i]);
        check_symbol(symbols[i], symbols[i], shifted[i]);
        check_timed_symbol(symbols[i], symbols[i], shifted[i]);
    }
    const uint8_t sym_positions[] = {10, 11, 12, 13, 16, 17, 18, 19, 28, 29};
    const uint16_t sym_plain[] = {
        KC_PERC, KC_MINS, KC_EQL, KC_DQT, KC_LPRN, KC_LCBR, KC_LBRC, KC_LT, LDQT, LSQT
    };
    const uint16_t sym_shifted[] = {
        KC_AT, KC_UNDS, KC_PLUS, KC_QUOT, KC_RPRN, KC_RCBR, KC_RBRC, KC_GT, RDQT, RSQT
    };
    for (size_t i = 0; i < ARRAY_SIZE(sym_positions); ++i) {
        check_symbol(keymaps[LAYER_SYM][0][sym_positions[i]], sym_plain[i], sym_shifted[i]);
        check_timed_symbol(keymaps[LAYER_SYM][0][sym_positions[i]], sym_plain[i], sym_shifted[i]);
    }
    check_symbol(LT(LAYER_RAT, RSQT), RSQT, NDSH);
    check_symbol(LSFT_T(KC_A), KC_A, LSFT(KC_A));
    check_symbol(RALT_T(KC_I), KC_I, LSFT(KC_I));
    check_timed_symbol(LSFT_T(KC_A), KC_A, LSFT(KC_A));
    check_timed_symbol(RALT_T(KC_I), KC_I, LSFT(KC_I));
    check_combo(LAYER_SYM, 11, 12, KC_TAB);
    check_combo(LAYER_SYM, 21, 22, CW_TOGG);
    check_combo(LAYER_SYM, 17, 18, KC_BSPC);
    check_combo(LAYER_SYM, 27, 28, KC_ENT);
    for (size_t i = 0; i < ARRAY_SIZE(key_combos); ++i)
        for (size_t j = 0; key_combos[i].input[j] != COMBO_END; ++j) {
            bool present = false;
            for (size_t layer = 0; layer < ARRAY_SIZE(keymaps); ++layer)
                for (size_t key = 0; key < MATRIX_COLS; ++key)
                    present |= keymaps[layer][0][key] == key_combos[i].input[j];
            assert(present);
        }
    keyrecord_t record = {0};
    assert(!get_auto_shifted_key(LT(LAYER_SYM, KC_SPC), &record));
    assert(!get_auto_shifted_key(LT(LAYER_NAV, KC_BSPC), &record));
    assert(!get_auto_shifted_key(MS_BTN1, &record));
    assert(!get_auto_shifted_key(DRGSCRL, &record));
    assert(keymaps[LAYER_RAT][0][26] == MS_BTN1);
    assert(keymaps[LAYER_RAT][0][27] == MS_BTN2);
    assert(keymaps[LAYER_RAT][0][28] == DRGSCRL);
}
"""
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "test.c"
            binary = Path(directory) / "test"
            source.write_text(harness)
            subprocess.run(
                [
                    "cc", "-std=gnu11", "-Wall", "-Werror",
                    "-Wno-unused-function", "-I", str(FIRMWARE / "quantum"),
                    "-I", str(FIRMWARE / "quantum/keymap_extras"),
                    "-I", str(FIRMWARE / "quantum/sequencer"),
                    str(source), "-o", str(binary),
                ],
                check=True,
            )
            subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    unittest.main()
