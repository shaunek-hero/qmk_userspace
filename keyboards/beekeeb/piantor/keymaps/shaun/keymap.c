// Piantor (42-key) keymap — converted from Vial (piantor-2026-08-13.vil)
#include QMK_KEYBOARD_H
#include "keymap_introspection.h"  // tap_dance_get()

enum layers {
    _BASE = 0,
    _UNUSED1,  // empty in Vial; kept so SYM/NAV/NUM keep their original indices
    _UNUSED2,
    _SYM,      // 3
    _NAV,      // 4
    _NUM,      // 5
};

// ---------------------------------------------------------------------------
// Tap dance: tap for one keycode, hold for another.
// QMK has no built-in tap-hold dance, so this is the pattern from the QMK docs.
// ---------------------------------------------------------------------------

typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

enum tap_dance_keys {
    TD_1_F1 = 0,
    TD_2_F2,
    TD_3_F3,
    TD_4_F4,
    TD_5_F5,
    TD_6_F6,
    TD_7_F7,
    TD_8_F8,
    TD_9_F9,
    TD_AST_F10,
    TD_0_F11,
    TD_SLSH_F12,
    TD_ESC_CAPS,
};

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            // With PERMISSIVE_HOLD on, a key pressed and released mid-hold
            // should still resolve to the hold action, so skip this check.
            && !state->interrupted
#endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

// fn members are {on_each_tap, on_dance_finished, on_reset, on_each_release}
#define ACTION_TAP_DANCE_TAP_HOLD(tap_kc, hold_kc)                                        \
    {                                                                                     \
        .fn        = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset, NULL}, \
        .user_data = (void *)&((tap_dance_tap_hold_t){tap_kc, hold_kc, 0}),               \
    }

tap_dance_action_t tap_dance_actions[] = {
    [TD_1_F1]     = ACTION_TAP_DANCE_TAP_HOLD(KC_1, KC_F1),
    [TD_2_F2]     = ACTION_TAP_DANCE_TAP_HOLD(KC_2, KC_F2),
    [TD_3_F3]     = ACTION_TAP_DANCE_TAP_HOLD(KC_3, KC_F3),
    [TD_4_F4]     = ACTION_TAP_DANCE_TAP_HOLD(KC_4, KC_F4),
    [TD_5_F5]     = ACTION_TAP_DANCE_TAP_HOLD(KC_5, KC_F5),
    [TD_6_F6]     = ACTION_TAP_DANCE_TAP_HOLD(KC_6, KC_F6),
    [TD_7_F7]     = ACTION_TAP_DANCE_TAP_HOLD(KC_7, KC_F7),
    [TD_8_F8]     = ACTION_TAP_DANCE_TAP_HOLD(KC_8, KC_F8),
    [TD_9_F9]     = ACTION_TAP_DANCE_TAP_HOLD(KC_9, KC_F9),
    [TD_AST_F10]  = ACTION_TAP_DANCE_TAP_HOLD(KC_ASTR, KC_F10),
    [TD_0_F11]    = ACTION_TAP_DANCE_TAP_HOLD(KC_0, KC_F11),
    [TD_SLSH_F12] = ACTION_TAP_DANCE_TAP_HOLD(KC_SLSH, KC_F12),
    [TD_ESC_CAPS] = ACTION_TAP_DANCE_TAP_HOLD(KC_ESC, KC_CAPS),
};

// Emit the tap keycode immediately on a quick tap, instead of waiting out the
// tapping term. Without this, tapping a tap-dance key feels laggy.
//
// QMK 0.33 moved tap dance state out of tap_dance_action_t to save memory, so
// action and state are fetched separately (see ChangeLog/20251130.md).
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (IS_QK_TAP_DANCE(keycode) && !record->event.pressed) {
        uint8_t             idx    = QK_TAP_DANCE_GET_INDEX(keycode);
        tap_dance_action_t *action = tap_dance_get(idx);
        tap_dance_state_t  *state  = tap_dance_get_state(idx);
        if (action != NULL && state != NULL && state->count && !state->finished) {
            tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
            tap_code16(tap_hold->tap);
        }
    }
    return true;
}

// Vial used a 300 ms tapping term for every tap dance, but the global term
// applies to the home-row mods.
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    if (IS_QK_TAP_DANCE(keycode)) {
        return 300;
    }
    return TAPPING_TERM;
}

// ---------------------------------------------------------------------------
// Combos
// ---------------------------------------------------------------------------

const uint16_t PROGMEM combo_m_comm[] = {KC_M, KC_COMM, COMBO_END};
const uint16_t PROGMEM combo_c_v[]    = {KC_C, KC_V, COMBO_END};

combo_t key_combos[] = {
    COMBO(combo_m_comm, OSM(MOD_HYPR)),
    COMBO(combo_c_v, OSM(MOD_HYPR)),
};

// ---------------------------------------------------------------------------
// Keymap
// ---------------------------------------------------------------------------

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // Base: QWERTY with home-row mods (Ctrl/Alt/Gui/Shift, mirrored)
    [_BASE] = LAYOUT_split_3x6_3(
        KC_TAB,          KC_Q,         KC_W,         KC_E,         KC_R,         KC_T,
                                                                                        KC_Y,         KC_U,         KC_I,         KC_O,         KC_P,         KC_BSPC,
        TD(TD_ESC_CAPS), LCTL_T(KC_A), LALT_T(KC_S), LGUI_T(KC_D), LSFT_T(KC_F), LT(_NUM, KC_G),
                                                                                        KC_H,         RSFT_T(KC_J), RGUI_T(KC_K), RALT_T(KC_L), RCTL_T(KC_SCLN), KC_QUOT,
        KC_UNDS,         KC_Z,         KC_X,         KC_C,         KC_V,         KC_B,
                                                                                        KC_N,         KC_M,         KC_COMM,      KC_DOT,       KC_SLSH,      KC_MINS,
                                       MO(_NUM),     MO(_NUM),     MO(_NAV),
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

    // Number pad / F-keys — tap a key for the digit, hold it for the F-key
    [_NUM] = LAYOUT_split_3x6_3(
        _______, _______, _______, _______, _______, _______,    KC_EQL,  TD(TD_7_F7), TD(TD_8_F8), TD(TD_9_F9), TD(TD_AST_F10), _______,
        _______, KC_LCTL, KC_LALT, KC_LGUI, KC_LSFT, KC_EQL,     KC_PLUS, TD(TD_4_F4), TD(TD_5_F5), TD(TD_6_F6), TD(TD_0_F11),   _______,
        _______, _______, _______, _______, _______, _______,    KC_MINS, TD(TD_1_F1), TD(TD_2_F2), TD(TD_3_F3), TD(TD_SLSH_F12), _______,
                          _______, _______, _______,             _______, _______, KC_P0
    ),
};
