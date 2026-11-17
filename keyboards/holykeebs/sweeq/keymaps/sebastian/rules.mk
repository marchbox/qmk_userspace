VIA_ENABLE = yes
VIAL_ENABLE = yes
# Keep Vial's global tap-hold settings for mod-taps, but specialize layer-taps.
$(INTERMEDIATE_OUTPUT)/quantum/qmk_settings.o: private CFLAGS += \
    -Dqs_get_tapping_term=vial_qs_get_tapping_term \
    -Dget_hold_on_other_key_press=vial_get_hold_on_other_key_press \
    -Dget_quick_tap_term=vial_get_quick_tap_term \
    -Dget_retro_tapping=vial_get_retro_tapping \
    -Dget_chordal_hold=vial_get_chordal_hold \
    -Dget_flow_tap_term=vial_get_flow_tap_term
# Vial's non-weak Auto Shift predicate does not recognize mod-tap keycodes.
$(INTERMEDIATE_OUTPUT)/quantum/qmk_settings.o: private CFLAGS += -Dget_auto_shifted_key=vial_get_auto_shifted_key
# Attach Repeat Key bookkeeping to Vial's runtime key overrides.
$(INTERMEDIATE_OUTPUT)/quantum/vial.o: private CFLAGS += -Dkey_override_get=vial_key_override_get
# Upstream's RETRO_TAPPING_PER_KEY macro lacks parentheses around an &&.
$(INTERMEDIATE_OUTPUT)/quantum/action_tapping.o: private CFLAGS += -Wno-parentheses
USER_NAME = holykeebs
POINTING_DEVICE = trackpoint
POINTING_DEVICE_POSITION = right
