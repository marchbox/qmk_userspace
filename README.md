# QMK Userspace

This userspace contains **Leviathan**, a BastardKB Charybdis Nano, and
**Sebastian**, a Holykeebs Sweep (`holykeebs/sweeq`). They use separate firmware
forks and GitHub Actions builds.

You can read how to compile your own keymap on the official docs here: [https://docs.bastardkb.com/fw/compile-firmware.html](https://docs.bastardkb.com/fw/compile-firmware.html).

## Leviathan (Charybdis Nano)

The `leviathan` keymap identifies the keyboard as **Leviathan** over USB.

The existing `qmk.json` targets and release workflow build against
`bastardkb/bastardkb-qmk`. Sweep is intentionally not listed there because
that fork does not supply its Holykeebs firmware.

`modules/bastardkb` is pinned to
`0cda221c0952d4e25072393da506f358ec529ed8`, the last revision before
the pointing module acquired an unconditional Argos dependency.
This preserves the existing pointing API, static combos, and tap dances.
The submodule updater only advances Holykeebs; do not update BastardKB without
migrating and validating the Charybdis keymap.

Successful Leviathan builds publish numbered releases (`r1`, `r2`, ...),
following BastardKB's upstream convention. Failed builds do not publish.

## Sebastian (Holykeebs Sweep)

The `sebastian` keymap builds Vial firmware for a **right-side TrackPoint**,
without OLED or trackball lighting, and identifies the keyboard as
**Sebastian** over USB and in Vial. The shared Holykeebs userspace is pinned
as `modules/holykeebs` and exposed through `users/holykeebs`.

| Layer | Non-thumb keys |
| --- | --- |
| 0 | Charybdis `leviathan` base layer |
| 1 | Charybdis `leviathan` symbol layer |
| 2 | Saved custom Vial function/navigation layer |
| 3 | Saved custom Vial pointing/media layer |

The four thumbs follow the saved Vial layout: left outer one-shot Shift/layer 3,
left inner Space/layer 1, right inner Backspace/layer 2, and right outer
one-shot Ctrl.
The left outer thumb uses `LT(3, KC_LSFT)` with a keymap hook that makes
the tap one-shot Shift, rather than a tap dance. It uses the normal 175 ms
tapping term. Upper-layer thumbs are transparent.
TrackPoint movement automatically activates the existing pointing/media
layer 3, with a default 650 ms inactivity timeout. Auto mouse is enabled
on every boot; its Vial controls can toggle it during use and adjust the
timeout (save with `HK_SAVE` to retain timeout changes).
The existing layer-3 assignments, including disabled keys, are unchanged.
There is no extra layer 4; the Charybdis layer-4 taps become plain Z and
right single quote.
Charybdis combos and shifted-symbol handling are retained, with symbol
overrides limited to layers 0–1. The Vial screenshot tap dance, the lock/sleep
macro, and the saved Vial timing settings are firmware defaults and remain editable
in Vial. Holding layers 1 and 2 does not automatically enable layer 3.
Like the upstream Holykeebs firmware, this build uses insecure Vial mode:
configuration and matrix testing do not require an unlock combo.

To enter the UF2 bootloader, hold the right inner Backspace/layer-2 thumb
and press **F1 + F2 + F3 + F4** together (the first four keys of the top
row on the navigation layer).

Initialize dependencies and clone the correct firmware fork:

```sh
git submodule update --init --recursive
git clone --recurse-submodules -b hk-master https://github.com/holykeebs/vial-qmk.git /path/to/holykeebs-vial-qmk
```

Configure QMK to use that checkout and this userspace, then build:

```sh
qmk config user.qmk_home=/path/to/holykeebs-vial-qmk
qmk config user.overlay_dir="$PWD"
qmk compile -kb holykeebs/sweeq -km sebastian
```

These settings are global. Switch `user.qmk_home` back to your BastardKB
firmware checkout before building Charybdis.

The result is `holykeebs_sweeq_sebastian.uf2`. Flash the **same image to both
halves**, and connect USB to the **right half**. Disconnect power before
connecting or disconnecting TRRS.

Firmware defaults are loaded when Vial's EEPROM is invalid (including a new
build). An existing left outer thumb assigned to `TD(1)` is automatically
migrated to the new Shift/layer-3 key; other Vial edits are preserved.
If an existing Vial configuration is retained, add the new
`F1 + F2 + F3 + F4` -> `QK_BOOT` combo in Vial, or restore firmware defaults.
To restore everything later, use QMK's `QK_CLEAR_EEPROM` keycode,
which can be assigned in Vial. Vial's keymap-only reset clears dynamic combos
and tap dances rather than restoring their baked defaults.

GitHub Actions builds Leviathan with `build_leviathan.yaml` and publishes
its numbered releases with `publish_leviathan.yml`. Sebastian is built with
`build_sebastian.yaml` and downloaded as the `Sebastian-Firmware` artifact.
Push builds only run when that keyboard's keymap, dependencies, or workflow
changes. Shared `.gitmodules` changes run both; README-only changes and tag
pushes run neither. Both builds can still be run manually in Actions.
Build details:
[Holykeebs firmware documentation](https://docs.holykeebs.com/firmware/).