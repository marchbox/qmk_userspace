# QMK Userspace

This userspace contains keymaps for a BastardKB Charybdis Nano and a
Holykeebs Sweep (`holykeebs/sweeq`). They use separate firmware forks and
GitHub Actions builds.

You can read how to compile your own keymap on the official docs here: [https://docs.bastardkb.com/fw/compile-firmware.html](https://docs.bastardkb.com/fw/compile-firmware.html).

## Charybdis Nano

The existing `qmk.json` targets and release workflow build against
`bastardkb/bastardkb-qmk`. Sweep is intentionally not listed there because
that fork does not supply its Holykeebs firmware.

## Holykeebs Sweep

The `leviathan` keymap builds Vial firmware for a **right-side TrackPoint**,
without OLED or trackball lighting. The shared Holykeebs userspace is pinned
as `modules/holykeebs` and exposed through `users/holykeebs`.

| Layer | Non-thumb keys |
| --- | --- |
| 0 | Charybdis `leviathan` base layer |
| 1 | Charybdis `leviathan` symbol layer |
| 2 | Saved custom Vial function/navigation layer |
| 3 | Saved custom Vial pointing/media layer |

The four thumbs match the saved Vial layout: left outer tap dance
(one-shot Shift on tap, layer 3 on hold), left inner Space/layer 1,
right inner Backspace/layer 2, and right outer one-shot Ctrl.
Upper-layer thumbs are transparent. There is no mouse layer or auto-mouse
activation; the Charybdis layer-4 taps become plain Z and right single quote.
Charybdis combos and shifted-symbol handling are retained, with symbol
overrides limited to layers 0–1. Vial tap dances, the lock/sleep macro,
and the saved Vial timing settings are firmware defaults and remain editable
in Vial. Holding layers 1 and 2 does not automatically enable layer 3.
Like the upstream Holykeebs firmware, this build uses insecure Vial mode:
configuration and matrix testing do not require an unlock combo.

Initialize dependencies and clone the correct firmware fork:

```sh
git submodule update --init --recursive
git clone --recurse-submodules -b hk-master https://github.com/holykeebs/vial-qmk.git /path/to/holykeebs-vial-qmk
```

Configure QMK to use that checkout and this userspace, then build:

```sh
qmk config user.qmk_home=/path/to/holykeebs-vial-qmk
qmk config user.overlay_dir="$PWD"
qmk compile -kb holykeebs/sweeq -km leviathan
```

These settings are global. Switch `user.qmk_home` back to your BastardKB
firmware checkout before building Charybdis.

The result is `holykeebs_sweeq_leviathan.uf2`. Flash the **same image to both
halves**, and connect USB to the **right half**. Disconnect power before
connecting or disconnecting TRRS.

Firmware defaults are loaded when Vial's EEPROM is invalid (including a new
build). To restore everything later, use QMK's `QK_CLEAR_EEPROM` keycode,
which can be assigned in Vial. Vial's keymap-only reset clears dynamic combos
and tap dances rather than restoring their baked defaults.

GitHub Actions provides the Sweep image as the separate `Sweep-Firmware`
artifact; the Charybdis release workflow remains unchanged. Build details:
[Holykeebs firmware documentation](https://docs.holykeebs.com/firmware/).