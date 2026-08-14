#pragma once

// Settings carried over from the Vial "QMK Settings" tab.
//
// Two of the Vial options have no direct equivalent in modern QMK:
//
//   "Ignore Mod Tap Interrupt" (checked) — became the DEFAULT mod-tap behavior
//       in QMK 0.21 (see docs/ChangeLog/20230528.md). Defining the old macro is
//       now a hard #error. Opting *out* would mean HOLD_ON_OTHER_KEY_PRESS, so
//       leaving it undefined is what reproduces the checked box.
//
//   "Tapping Force Hold" (unchecked) — replaced by QUICK_TAP_TERM, which
//       defaults to TAPPING_TERM. Enabling the old option == QUICK_TAP_TERM 0,
//       so leaving it undefined reproduces the unchecked box.
//
// "Retro Tapping" was unchecked, and RETRO_TAPPING is off by default.

#define TAPPING_TERM 200
#define PERMISSIVE_HOLD

// Home row mods: settle a mod-tap as *tapped* when the next key is on the same
// hand, which is what kills accidental mods during fast rolls. Pairs with
// PERMISSIVE_HOLD, which governs the opposite-hands case.
//
// Two things to know:
//   - It has no effect after TAPPING_TERM, so deliberately holding a mod past
//     200 ms still gives a same-hand chord.
//   - Same-hand shortcuts (Cmd+C is D and C, both left) now want the mirrored
//     mod instead: hold RGUI_T(KC_K) on the right, press C on the left.
//
// Handedness needs no table here — QMK derives it from the split geometry in
// keyboard.json, correctly for LAYOUT_split_3x6_3 (left 'L', right 'R').
#define CHORDAL_HOLD

// Caps Word shifts the next word,
// then drops back to lowercase on space/punctuation or after this many ms idle.
#define CAPS_WORD_IDLE_TIMEOUT 5000
