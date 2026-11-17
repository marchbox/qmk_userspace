VIA_ENABLE = yes
VIAL_ENABLE = yes
# Vial's non-weak Auto Shift predicate does not recognize mod-tap keycodes.
$(INTERMEDIATE_OUTPUT)/quantum/qmk_settings.o: private CFLAGS += -Dget_auto_shifted_key=vial_get_auto_shifted_key
# Upstream's RETRO_TAPPING_PER_KEY macro lacks parentheses around an &&.
$(INTERMEDIATE_OUTPUT)/quantum/action_tapping.o: private CFLAGS += -Wno-parentheses
USER_NAME = holykeebs
POINTING_DEVICE = trackpoint
POINTING_DEVICE_POSITION = right
