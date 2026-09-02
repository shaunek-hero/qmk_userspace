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
//
// The thumbs come out of that derivation as L L L R R R, which is wrong for
// the two thumb layer-taps — see get_chordal_hold() in keymap.c, which exempts
// them so same-hand chords like SYM+I (`*`) work.
#define CHORDAL_HOLD

// Flow Tap: if a tap-hold key is pressed within this many ms of the previous
// key, force the tap. In effect it switches mod-taps off during fast typing,
// which is the case CHORDAL_HOLD can't cover — an opposite-hand roll like the
// `/` in `/Applications` followed by `A`, which PERMISSIVE_HOLD would
// otherwise resolve as a hold and fire Hyper+A.
//
// The default is_flow_tap_key() covers KC_A-KC_Z and KC_SLSH, so z, / and
// every home row mod are in scope. The two Shifts are then exempted again in
// keymap.c via get_flow_tap_term(), because capitalising mid-sentence is
// always a fast roll and was turning `I` into `fi`.
//
// The trade: mid-flow mod chords no longer work. Cmd+C while typing fast
// wants a brief pause first. 150 ms is QMK's suggested starting point; lower
// it if it feels obstructive, raise it if stray mods still get through.
#define FLOW_TAP_TERM 150

// Caps Word shifts the next word,
// then drops back to lowercase on space/punctuation or after this many ms idle.
#define CAPS_WORD_IDLE_TIMEOUT 5000
