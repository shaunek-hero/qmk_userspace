// Piantor (42-key) keymap — converted from Vial (piantor-2026-08-14.vil)
//
// Deliberate deviations from the .vil:
//   - The Esc key is plain KC_ESC rather than a tap-Esc/hold-CapsLock tap
//     dance, so nothing here needs custom C. Its two former roles live on
//     layers under the same physical key: Caps Word on _NAV, Caps Lock on
//     _NUM.
//
// Hyper (for aerospace app switching) sits on the right thumb as a one-shot,
// and on Z and / as ALL_T hold-for-Hyper mod-taps.
//
// Watch hold-/ while typing paths: `/` then a left-hand letter is an
// opposite-hand roll, which PERMISSIVE_HOLD resolves as a hold, so
// `/Applications` and `/System` can fire Hyper+A and Hyper+S and launch apps
// rather than merely mistyping. CHORDAL_HOLD does not catch this — it only
// suppresses same-hand rolls. FLOW_TAP_TERM is the config-only fix if needed.
#include QMK_KEYBOARD_H

enum layers {
    _BASE = 0,
    _SYM,  // 1
    _NAV,  // 2
    _NUM,  // 3
    _FN,   // 4 — reached from _NUM via the inner right thumb
};

// Turn Flow Tap off for the two Shifts, leaving it at FLOW_TAP_TERM for
// everything else.
//
// Capitalising mid-sentence is inherently a fast roll: Shift lands within the
// flow term of the previous letter, so Flow Tap forced the tap and `I` came out
// as `fi`, `?` as `f/`. Shift is also the safest mod to exempt — a stray Shift
// is a capital letter, whereas a stray Gui or Ctrl fires a shortcut. CHORDAL_HOLD
// still suppresses same-hand rolls on these keys.
uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prev_keycode) {
    if (is_flow_tap_key(keycode) && is_flow_tap_key(prev_keycode)) {
        switch (keycode) {
            case LSFT_T(KC_F):
            case RSFT_T(KC_J):
                return 0;  // never suppress Shift
            default:
                return FLOW_TAP_TERM;
        }
    }
    return 0;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // Base: QWERTY with home-row mods (Ctrl/Alt/Gui/Shift, mirrored)
    [_BASE] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_Q,         KC_W,         KC_E,         KC_R,         KC_T,
                                                                                 KC_Y,         KC_U,         KC_I,         KC_O,         KC_P,            KC_BSPC,
        KC_ESC,  LCTL_T(KC_A), LALT_T(KC_S), LGUI_T(KC_D), LSFT_T(KC_F), LT(_NUM, KC_G),
                                                                                 KC_H,         RSFT_T(KC_J), RGUI_T(KC_K), RALT_T(KC_L), RCTL_T(KC_SCLN), KC_QUOT,
        KC_UNDS, ALL_T(KC_Z),  KC_X,         KC_C,         KC_V,         KC_B,
                                                                                 KC_N,         KC_M,         KC_COMM,      KC_DOT,       ALL_T(KC_SLSH),  KC_MINS,
                               MO(_NUM),     MO(_NUM),     MO(_NAV),
                                                                                 LT(_SYM, KC_ENT), LT(_NUM, KC_SPC), OSM(MOD_HYPR)
    ),

    // Symbols — held via the inner right thumb
    [_SYM] = LAYOUT_split_3x6_3(
        _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,    KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, _______,
        _______, KC_TILD, KC_LCBR, KC_LPRN, KC_LBRC, KC_EQL,     KC_PLUS, KC_RBRC, KC_RPRN, KC_RCBR, KC_PIPE, KC_BSLS,
        _______, KC_GRV,  _______, _______, _______, _______,    _______, _______, _______, _______, KC_BSLS, _______,
                          _______, _______, _______,             _______, _______, _______
    ),

    // Nav / media / clipboard — held via the inner left thumb
    [_NAV] = LAYOUT_split_3x6_3(
        SGUI(KC_W), KC_MUTE,    KC_MRWD,    KC_MFFD,    KC_MPLY,    KC_VOLU,    _______, _______, _______, _______, _______, _______,
        CW_TOGG,    KC_LCTL,    KC_LALT,    KC_LGUI,    KC_LSFT,    KC_VOLD,    KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______,
        _______,    LGUI(KC_Z), LGUI(KC_X), LGUI(KC_C), LGUI(KC_V), LGUI(KC_Y), KC_HOME, KC_PGDN, KC_PGUP, KC_END,  _______, _______,
                                LCG(KC_Q),  _______,    _______,                _______, _______, _______
    ),

    // Number pad. The `/` slot is transparent on purpose so it falls through to
    // KC_SLSH on the base layer; _FN puts F12 there instead.
    [_NUM] = LAYOUT_split_3x6_3(
        _______, _______, _______, _______, _______, _______,    KC_EQL,  KC_7, KC_8, KC_9, KC_ASTR, _______,
        KC_CAPS, KC_LCTL, KC_LALT, KC_LGUI, KC_LSFT, KC_EQL,     KC_PLUS, KC_4, KC_5, KC_6, KC_0,    _______,
        _______, _______, _______, _______, _______, _______,    KC_MINS, KC_1, KC_2, KC_3, _______, _______,
                          _______, _______, _______,             MO(_FN), _______, KC_P0
    ),

    // F-keys, laid out over the same positions as the number pad.
    // Reached by holding _NUM, then the inner right thumb.
    [_FN] = LAYOUT_split_3x6_3(
        _______, _______, _______, _______, _______, _______,    _______, KC_F7, KC_F8, KC_F9, KC_F10, _______,
        _______, _______, _______, _______, _______, _______,    _______, KC_F4, KC_F5, KC_F6, KC_F11, _______,
        _______, _______, _______, _______, _______, _______,    _______, KC_F1, KC_F2, KC_F3, KC_F12, _______,
                          _______, _______, _______,             _______, _______, _______
    ),
};
