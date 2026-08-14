// Piantor (42-key) keymap — converted from Vial (piantor-2026-08-14.vil)
//
// Deliberate deviations from the .vil:
//   - Outer left thumb is Caps Word instead of a second, duplicated MO(_NUM).
//   - The Esc key is plain KC_ESC rather than a tap-Esc/hold-CapsLock tap
//     dance; Caps Word covers the shifting, so nothing here needs custom C.
#include QMK_KEYBOARD_H

enum layers {
    _BASE = 0,
    _UNUSED1,  // empty in Vial; kept so the layers below keep their original indices
    _UNUSED2,
    _SYM,      // 3
    _NAV,      // 4
    _NUM,      // 5
    _FN,       // 6 — reached from _NUM via the inner right thumb
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // Base: QWERTY with home-row mods (Ctrl/Alt/Gui/Shift, mirrored)
    [_BASE] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_Q,         KC_W,         KC_E,         KC_R,         KC_T,
                                                                                 KC_Y,         KC_U,         KC_I,         KC_O,         KC_P,            KC_BSPC,
        KC_ESC,  LCTL_T(KC_A), LALT_T(KC_S), LGUI_T(KC_D), LSFT_T(KC_F), LT(_NUM, KC_G),
                                                                                 KC_H,         RSFT_T(KC_J), RGUI_T(KC_K), RALT_T(KC_L), RCTL_T(KC_SCLN), KC_QUOT,
        KC_UNDS, KC_Z,         KC_X,         KC_C,         KC_V,         KC_B,
                                                                                 KC_N,         KC_M,         KC_COMM,      KC_DOT,       KC_SLSH,         KC_MINS,
                               CW_TOGG,      MO(_NUM),     MO(_NAV),
                                                                                 LT(_SYM, KC_ENT), LT(_NUM, KC_SPC), OSM(MOD_HYPR)
    ),

    [_UNUSED1] = LAYOUT_split_3x6_3(
        _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, _______,
                          _______, _______, _______,             _______, _______, _______
    ),

    [_UNUSED2] = LAYOUT_split_3x6_3(
        _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, _______,
                          _______, _______, _______,             _______, _______, _______
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
        _______,    KC_LCTL,    KC_LALT,    KC_LGUI,    KC_LSFT,    KC_VOLD,    KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______,
        _______,    LGUI(KC_Z), LGUI(KC_X), LGUI(KC_C), LGUI(KC_V), LGUI(KC_Y), KC_HOME, KC_PGDN, KC_PGUP, KC_END,  _______, _______,
                                LCG(KC_Q),  _______,    _______,                _______, _______, _______
    ),

    // Number pad. The `/` slot is transparent on purpose so it falls through to
    // KC_SLSH on the base layer; _FN puts F12 there instead.
    [_NUM] = LAYOUT_split_3x6_3(
        _______, _______, _______, _______, _______, _______,    KC_EQL,  KC_7, KC_8, KC_9, KC_ASTR, _______,
        _______, KC_LCTL, KC_LALT, KC_LGUI, KC_LSFT, KC_EQL,     KC_PLUS, KC_4, KC_5, KC_6, KC_0,    _______,
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
