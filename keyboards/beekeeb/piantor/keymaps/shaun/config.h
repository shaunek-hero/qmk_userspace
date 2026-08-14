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

#define COMBO_TERM 50
