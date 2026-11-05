// Cheapino v2 (36-key) keymap -- the Piantor keymap without its outer columns.
//
// The layers and every callback below are carried over from
// keyboards/beekeeb/piantor/keymaps/shaun, unchanged apart from the layout
// macro. Dropping the outer columns leaves these keys needing a new home:
//
//   Tab             Left outer thumb, which was blank on the Piantor.
//   Backspace       Right outer thumb, which was OSM(MOD_HYPR) on the Piantor.
//                   Also on _NAV, over `;`, where it sits on the Sofle.
//   Hyper           Hold G, hold H or hold either outer thumb. G and H come
//                   from the Sofle. Trying them alongside the thumbs is
//                   deliberate: once one route wins, the others should go.
//   Esc, ', -, _    Combos, the ones already used on the Sofle. `"` comes along
//                   too, because Shift+' would otherwise need three keys.
//   Caps Word       _NAV, over U.
//   Shift+Cmd+W     Not bound anywhere: it was the _NAV outer column key.
//
// Hold-Backspace is Hyper, so repeating Backspace takes a tap and then a hold
// inside QUICK_TAP_TERM, or holding it on _NAV.
//
// The outer thumbs get no Flow Tap protection, since KC_TAB and KC_BSPC are
// outside QMK's default is_flow_tap_key(). G and H are letters, so they are
// covered. The case to watch is correcting a typo: Backspace held through a
// left-hand letter is an opposite-hand press, which PERMISSIVE_HOLD can resolve
// as Hyper+letter.
//
// Encoder actions are not in this file: see keyboards/cheapino/encoder.c.
#include QMK_KEYBOARD_H

enum layers {
    _BASE = 0,
    _SYM,  // 1
    _NAV,  // 2
    _NUM,  // 3
    _FN,   // 4 — tri-layer: _NUM + _NAV, i.e. the left middle and inner thumbs
};

const uint16_t PROGMEM combo_dquo[] = {KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM combo_esc[]  = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM combo_unds[] = {LSFT_T(KC_F), ALL_T(KC_G), COMBO_END};
const uint16_t PROGMEM combo_mins[] = {ALL_T(KC_H), RSFT_T(KC_J), COMBO_END};
const uint16_t PROGMEM combo_quot[] = {RGUI_T(KC_K), RALT_T(KC_L), COMBO_END};

combo_t key_combos[] = {
    COMBO(combo_dquo, KC_DQUO),
    COMBO(combo_esc, KC_ESC),
    COMBO(combo_unds, KC_UNDS),
    COMBO(combo_mins, KC_MINS),
    COMBO(combo_quot, KC_QUOT),
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

// No auto-repeat on the Enter and Space thumbs.
//
// Quick tap exists so a mod-tap can repeat its letter: tap `a`, press and hold
// it again inside QUICK_TAP_TERM, get `aaaa` rather than Ctrl. Worth keeping on
// the home row. On the thumbs the tap keycodes are Enter and Space, which are
// not keys worth auto-repeating that way, and the cost is real — tapping Space
// and then reaching for a key on its layer within 200 ms repeated the space
// instead, because quick tap fires before the layer ever comes up.
//
// It only triggers when the immediately preceding action was an uninterrupted
// tap of this same key (action_tapping.c:604), which is why it showed up after
// Space and not mid-word.
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case RSFT_T(KC_ENT):
        case LT(_SYM, KC_SPC):
            return 0;
    }
    return QUICK_TAP_TERM;
}

// Let the Enter and Space thumbs chord with their own hand.
//
// CHORDAL_HOLD settles a tap-hold as *tapped* when the next key is on the same
// hand, which is exactly what protects the home row mods. Applied to a thumb
// layer key it is actively wrong. _SYM is reachable only through
// LT(_SYM, KC_SPC), and QMK's generated handedness map puts the thumbs at
// L L L R R R -- so that key is plain 'R' with no exemption, every right-hand
// symbol was a same-hand chord, and it settled as a tap: `*` (I) and `+` (H)
// came out as Space followed by the base letter. It only bites inside
// TAPPING_TERM, which is why holding the thumb deliberately always worked and
// fast rolls did not.
//
// RSFT_T(KC_ENT) is on the neighbouring thumb and has the same flaw: a quick
// right-hand capital would emit a Return and then the lowercase letter.
//
// QMK's docs suggest '*' handedness for exactly this case (docs/tap_hold.md).
// Overriding per-chord rather than per-key keeps this in terms of keycodes
// instead of matrix positions, which the docs warn are irregular around thumb
// clusters. Everything else falls through to the default, so the home row mods
// keep their same-hand protection.
bool get_chordal_hold(uint16_t tap_hold_keycode, keyrecord_t *tap_hold_record,
                      uint16_t other_keycode, keyrecord_t *other_record) {
    switch (tap_hold_keycode) {
        case RSFT_T(KC_ENT):
        case LT(_SYM, KC_SPC):
            return true;
    }
    return get_chordal_hold_default(tap_hold_record, other_record);
}

// _FN is a tri-layer: hold the left middle and inner thumbs.
//
// Those are MO(_NUM) and MO(_NAV), so holding the pair turns on both layers and
// update_tri_layer_state() lights _FN on top of them. _NUM and _NAV each leave
// the other's thumb transparent, so the order of the two presses does not matter.
//
// _FN is the highest layer number, so its keys win over the _NAV and _NUM
// definitions underneath while all three are on.
layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _NAV, _NUM, _FN);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // Base: QWERTY with home-row mods (Ctrl/Alt/Gui/Shift, mirrored)
    [_BASE] = LAYOUT_split_3x5_3(
        KC_Q,         KC_W,         KC_E,         KC_R,         KC_T,           KC_Y,        KC_U,         KC_I,         KC_O,         KC_P,
        LCTL_T(KC_A), LALT_T(KC_S), LGUI_T(KC_D), LSFT_T(KC_F), ALL_T(KC_G),    ALL_T(KC_H), RSFT_T(KC_J), RGUI_T(KC_K), RALT_T(KC_L), RCTL_T(KC_SCLN),
        KC_Z,         KC_X,         KC_C,         KC_V,         KC_B,           KC_N,        KC_M,         KC_COMM,      KC_DOT,       KC_SLSH,
                                    ALL_T(KC_TAB), MO(_NUM), MO(_NAV),          RSFT_T(KC_ENT), LT(_SYM, KC_SPC), ALL_T(KC_BSPC)
    ),

    // Symbols — held via the middle right thumb
    [_SYM] = LAYOUT_split_3x5_3(
        KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,    KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN,
        KC_TILD, KC_LCBR, KC_LPRN, KC_LBRC, KC_EQL,     KC_PLUS, KC_RBRC, KC_RPRN, KC_RCBR, KC_PIPE,
        KC_GRV,  _______, _______, _______, _______,    _______, _______, _______, _______, KC_BSLS,
                          _______, _______, _______,    _______, _______, _______
    ),

    // Nav / media / clipboard — held via the inner left thumb
    //
    // Browser back/forward sit over Y and O as Cmd+[ / Cmd+], not QMK's
    // KC_WWW_BACK / KC_WWW_FORWARD because I'm primarily on macos
    [_NAV] = LAYOUT_split_3x5_3(
        KC_MUTE,    KC_MRWD,    KC_MFFD,    KC_MPLY,    KC_VOLU,    LGUI(KC_LBRC), CW_TOGG, _______, LGUI(KC_RBRC), _______,
        KC_LCTL,    KC_LALT,    KC_LGUI,    KC_LSFT,    KC_VOLD,    KC_LEFT,       KC_DOWN, KC_UP,   KC_RGHT,       KC_BSPC,
        LGUI(KC_Z), LGUI(KC_X), LGUI(KC_C), LGUI(KC_V), LGUI(KC_Y), KC_HOME,       KC_PGDN, KC_PGUP, KC_END,        _______,
                                LCG(KC_Q),  _______,    _______,    _______,       _______, _______
    ),

    // Number pad. The `/` slot is transparent on purpose so it falls through to
    // KC_SLSH on the base layer; _FN puts F12 there instead.
    [_NUM] = LAYOUT_split_3x5_3(
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,       KC_6,    KC_7, KC_8, KC_9, KC_0,
        KC_LCTL, KC_LALT, KC_LGUI, KC_LSFT, KC_EQL,     KC_PLUS, KC_4, KC_5, KC_6, KC_0,
        _______, _______, _______, _______, _______,    KC_MINS, KC_1, KC_2, KC_3, _______,
                          _______, _______, _______,    _______, _______, KC_P0
    ),

    // F-keys, laid out over the same positions as the number pad.
    // Reached by holding the left middle and inner thumbs (see
    // layer_state_set_user above).
    [_FN] = LAYOUT_split_3x5_3(
        _______, _______, _______, _______, _______,    _______, KC_F7, KC_F8, KC_F9, KC_F10,
        _______, _______, _______, _______, _______,    _______, KC_F4, KC_F5, KC_F6, KC_F11,
        _______, _______, _______, _______, _______,    _______, KC_F1, KC_F2, KC_F3, KC_F12,
                          _______, _______, _______,    _______, _______, _______
    ),
};
