#include "hdo.h"

uint8_t is_master;

#ifdef OLED_DRIVER_ENABLE
#    define KEYLOGGER_LENGTH 10
static uint32_t oled_timer                       = 0;
static char     keylog_str[KEYLOGGER_LENGTH + 1] = {"\n"};
static uint16_t log_timer                        = 0;
// clang-format off
static const char PROGMEM code_to_name[0xFF] = {
//   0    1    2    3    4    5    6    7    8    9    A    B    c    D    E    F
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',  // 0x
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',  // 1x
    '3', '4', '5', '6', '7', '8', '9', '0',  20,  19,  27,  26,  22, '-', '=', '[',  // 2x
    ']','\\', '#', ';','\'', '`', ',', '.', '/', 128, ' ', ' ', ' ', ' ', ' ', ' ',  // 3x
    ' ', ' ', ' ', ' ', ' ', ' ', 'P', 'S', ' ', ' ', ' ', ' ',  16, ' ', ' ', ' ',  // 4x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 5x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 6x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 7x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 8x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 9x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Ax
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Bx
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Cx
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Dx
    'C', 'S', 'A', 'C', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Ex
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '        // Fx
};

void add_keylog(uint16_t keycode);
#endif

#ifndef UNICODE_ENABLE
#    define UC(x) KC_NO
#endif

/*
 * The `LAYOUT_kyria_base` macro is a template to allow the use of identical
 * modifiers for the default layouts (eg QWERTY, Colemak, Dvorak, etc), so
 * that there is no need to set them up for each layout, and modify all of
 * them if I want to change them.  This helps to keep consistency and ease
 * of use. K## is a placeholder to pass through the individual keycodes
 */

// clang-format off
#define LAYOUT_kyria_base( \
    K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, \
    K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, \
    K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A  \
  ) \
  LAYOUT_wrapper( \
      KC_ESC,         K01, K02,  K03,     K04,     K05,                                             K06,     K07,     K08,     K09,     K0A,     KC_BSLS, \
      LCTL_T(KC_TAB), K11, K12,  K13,     K14,     K15,                                             K16,     K17,     K18,     K19,     K1A, KC_ENT, \
      OS_LSFT, CTL_T(K21), K22,  K23,     K24,     K25,     KC_LEFT,   KC_DOWN,  KC_UP, KC_RIGHT,   K26,     K27,     K28,     K29, RCTL_T(K2A), OS_RSFT, \
                                 KC_MUTE, KC_LALT, KC_LGUI,  KC_SPC ,  DL_LWER,  BK_RAIS, KC_SPC,  KC_RGUI, LT(_FUNC, KC_DEL), KC_MPLY \
    )
/* Re-pass though to allow templates to be used */
#define LAYOUT_kyria_base_wrapper(...)       LAYOUT_kyria_base(__VA_ARGS__)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_QWERTY] = LAYOUT_kyria_base_wrapper(
        _________________QWERTY_L1_________________, _________________QWERTY_R1_________________,
        _________________QWERTY_L2_________________, _________________QWERTY_R2_________________,
        _________________QWERTY_L3_________________, _________________QWERTY_R3_________________
    ),
    [_MODS] = LAYOUT_wrapper(
        _______, ___________________BLANK___________________,                                       ___________________BLANK___________________, _______,
        _______, ___________________BLANK___________________,                                       ___________________BLANK___________________, _______,
        KC_LSFT, ___________________BLANK___________________, _______, _______,   _______, _______, ___________________BLANK___________________, KC_RSFT,
                                   _______, _______, KC_LALT, _______, _______,   _______, _______, KC_RGUI, _______, _______
    ),
    [_LOWER] = LAYOUT_wrapper(
        KC_GRV,  _________________LOWER_L1__________________,                                       _________________LOWER_R1__________________, S(KC_INS),
        _______, _________________LOWER_L2__________________,                                       _________________LOWER_R2__________________, KC_PIPE,
        _______, _________________LOWER_L3__________________, KC_HOME, KC_PGDN,   KC_PGUP, KC_END,  _________________LOWER_R3__________________, _______,
                                   _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______
    ),
    [_RAISE] = LAYOUT_wrapper(
        KC_GRV,  _________________RAISE_L1__________________,                                       _________________RAISE_R1__________________, KC_F11,
        _______, _________________RAISE_L2__________________,                                       _________________RAISE_R2__________________, KC_BSLS,
        _______, _________________RAISE_L3__________________, KC_HOME, KC_PGDN,   KC_PGUP, KC_END,  _________________RAISE_R3__________________, _______,
                                   _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______
    ),
    [_ADJUST] = LAYOUT_wrapper(
        KC_MAKE, _________________ADJUST_L1_________________,                                       _________________ADJUST_R1_________________, KC_RESET,
        VRSN,    _________________ADJUST_L2_________________,                                       _________________ADJUST_R2_________________, EEP_RST,
        _______, _________________ADJUST_L3_________________, HPT_TOG, HPT_FBK,   MG_NKRO, UC_MOD,  _________________ADJUST_R3_________________, TG_MODS,
                                   _______, _______, _______, KC_NUKE, _______,   _______, _______, _______, _______, _______),
    [_FUNC] = LAYOUT_wrapper(
        _______, ___________________BLANK___________________,                                       _______, _______, _______, _______, KC_LBRC, KC_RBRC,
        _______, ___________________BLANK___________________,                                       _______, _______, _______, _______, KC_QUOT, _______,
        _______, ___________________BLANK___________________, _______, _______,   _______, _______, _______, _______, _______, _______, _______, _______,
                                   _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______
    ),
//     [_LAYERINDEX] = LAYOUT_wrapper(
//       _______, _______, _______, _______, _______, _______,                                       _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                       _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______
//     ),
};
// clang-format on

bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
#ifdef OLED_DRIVER_ENABLE
        oled_timer = timer_read32();
        add_keylog(keycode);
#endif
#ifndef SPLIT_KEYBOARD
        if (keycode == RESET && !is_master) {
            return false;
        }
#endif
    }
    return true;
}
void matrix_init_keymap(void) { is_master = (uint8_t)is_keyboard_master(); }

#ifdef OLED_DRIVER_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_180; }

void add_keylog(uint16_t keycode) {
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) || (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX) || (keycode >= QK_MODS && keycode <= QK_MODS_MAX)) {
        keycode = keycode & 0xFF;
    } else if (keycode > 0xFF) {
        keycode = 0;
    }

    for (uint8_t i = (KEYLOGGER_LENGTH - 1); i > 0; --i) {
        keylog_str[i] = keylog_str[i - 1];
    }

    if (keycode < (sizeof(code_to_name) / sizeof(char))) {
        keylog_str[0] = pgm_read_byte(&code_to_name[keycode]);
    }

    log_timer = timer_read();
}

void update_log(void) {
    if (timer_elapsed(log_timer) > 750) {
        // add_keylog(0);
    }
}

void render_keylogger_status(void) {
    oled_write_P(PSTR("Keylogger: "), false);
    oled_write(keylog_str, false);
}

void render_default_layer_state(void) {
    oled_write_P(PSTR("Layout: "), false);
    switch (get_highest_layer(default_layer_state)) {
        case _QWERTY:  oled_write_ln_P(PSTR("Qwerty"), false); break;
    }
}

void render_layer_state(void) {
    oled_write_ln_P(PSTR("Layer:"), false);
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("Lower"), layer_state_is(_LOWER));
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("Raise"), layer_state_is(_RAISE));
    oled_write_P(PSTR(" "), false);
    oled_write_ln_P(PSTR("Mods"), layer_state_is(_MODS));
}

void render_keylock_status(uint8_t led_usb_state) {
    oled_write_P(PSTR("Lock: "), false);
    oled_write_P(PSTR("NUML"), led_usb_state & (1 << USB_LED_NUM_LOCK));
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("CAPS"), led_usb_state & (1 << USB_LED_CAPS_LOCK));
    oled_write_P(PSTR(" "), false);
    oled_write_ln_P(PSTR("SCLK"), led_usb_state & (1 << USB_LED_SCROLL_LOCK));
}

void render_mod_status(uint8_t modifiers) {
    oled_write_P(PSTR("Mods: "), false);
    oled_write_P(PSTR("SFT"), (modifiers & MOD_MASK_SHIFT));
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("CTL"), (modifiers & MOD_MASK_CTRL));
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("ALT"), (modifiers & MOD_MASK_ALT));
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("GUI"), (modifiers & MOD_MASK_GUI));
}

void render_bootmagic_status(void) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM logo[][2][3] = {
        {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
        {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
    };
    oled_write_P(PSTR("Boot  "), false);
    if (keymap_config.swap_lctl_lgui) {
        oled_write_P(logo[1][0], false);
    } else {
        oled_write_P(logo[0][0], false);
    }
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("NKRO"), keymap_config.nkro);
    oled_write_P(PSTR(" "), false);
    oled_write_ln_P(PSTR("GUI"), !keymap_config.no_gui);
    oled_write_P(PSTR("Magic "), false);
    if (keymap_config.swap_lctl_lgui) {
        oled_write_P(logo[1][1], false);
    } else {
        oled_write_P(logo[0][1], false);
    }
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("GRV"), keymap_config.swap_grave_esc);
    oled_write_P(PSTR("  "), false);
    oled_write_ln_P(PSTR("CAPS"), keymap_config.swap_control_capslock);
}

void render_user_status(void) {
    oled_write_P(PSTR("USER: "), false);
    oled_write_P(PSTR("Anim"), userspace_config.rgb_matrix_idle_anim);
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("Layr"), userspace_config.rgb_layer_change);
    oled_write_P(PSTR(" "), false);
    oled_write_ln_P(PSTR("Nuke"), userspace_config.nuke_switch);
}

// clang-format off
void render_logo(void) {
    static const char PROGMEM qmk_logo[] = {
        0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
        0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
        0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0};

    oled_write_P(qmk_logo, false);
}

#    ifndef SPLIT_TRANSPORT_MIRROR
static void render_hdo_logo(void) {
    static const char PROGMEM hdo_logo[] = {
        0x00, 0x00, 0x00, 0xf0, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0, 0xf0, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xf0, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00,
        0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xe0, 0xe0, 0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x00, 0x10, 0x10, 0x20,
        0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xc0, 0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf0, 0x70,
        0x70, 0x78, 0x78, 0x78, 0x78, 0x70, 0x70, 0xf0, 0xf0, 0xf0, 0xe0, 0xe0, 0xc0, 0xc0, 0x80, 0x80,
        0x00, 0x00, 0x10, 0x20, 0x40, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0x00, 0x00, 0x01, 0x01, 0xe1, 0xc3, 0x07, 0x07, 0x0f, 0x1f, 0x7e, 0xfc, 0xf8,
        0xf0, 0xc0, 0x01, 0x06, 0x0c, 0x30, 0xe0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
        0xe0, 0xf0, 0xf8, 0x7c, 0x3e, 0x1f, 0x0f, 0x07, 0x83, 0xc3, 0xe1, 0x71, 0x30, 0x38, 0x18, 0x08,
        0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x03, 0x07, 0x0f,
        0x1f, 0x3e, 0xfc, 0xf8, 0xf0, 0xe0, 0x81, 0x02, 0x0c, 0x38, 0xe0, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
        0x1f, 0xff, 0xff, 0xfc, 0x80, 0x00, 0x01, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xfe, 0xff,
        0xff, 0x0f, 0x01, 0x00, 0xc0, 0xf8, 0xfe, 0x1f, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x03, 0x0f, 0xff, 0xff, 0xfe, 0xe0, 0x00, 0x00, 0x07, 0xfe, 0xe0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x80, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xe0, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x80, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff,
        0xf8, 0x80, 0x00, 0x00, 0x1f, 0xff, 0xfb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0xfd, 0x3f, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x0f, 0x07, 0x07, 0x07, 0x87, 0x87, 0x07, 0x07, 0x07, 0x07,
        0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
        0x07, 0x0f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x03, 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfe,
        0x3f, 0x1f, 0x07, 0x01, 0x80, 0x60, 0x1c, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0f,
        0x3f, 0x7f, 0xfc, 0xf0, 0xe0, 0xc0, 0x83, 0x07, 0x0c, 0x18, 0x30, 0x20, 0x40, 0x00, 0x80, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
        0xc0, 0xe0, 0xf8, 0xfe, 0x7f, 0x1f, 0x0f, 0x03, 0x80, 0xe0, 0x38, 0x0f, 0x03, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x1f, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x7f, 0x7f, 0x7f, 0x7c, 0x38, 0x3c, 0x3c, 0x3e, 0x1e, 0x1f, 0x0f, 0x07, 0x07, 0x43, 0x21, 0x20,
        0x10, 0x08, 0x06, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0f, 0x0f, 0x1e, 0x3e, 0x3c, 0x3c, 0x78, 0x78, 0x78, 0x70,
        0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x78, 0x78, 0x78, 0x3c, 0x3c, 0x1e, 0x1f, 0x0f, 0x0f,
        0x07, 0x43, 0x41, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x0c, 0x08,
        0xf0, 0x00, 0x00, 0xc0, 0x30, 0xc8, 0x2c, 0x14, 0x04, 0x04, 0x04, 0x08, 0x10, 0xe0, 0x10, 0x80,
        0x00, 0x0c, 0x04, 0x04, 0x0c, 0x18, 0xf0, 0x10, 0x00, 0xe0, 0x10, 0xe8, 0x1c, 0x14, 0x04, 0x04,
        0x0c, 0x08, 0x30, 0xe0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x18, 0x1c, 0x12, 0x11,
        0x10, 0x11, 0x00, 0x03, 0x06, 0x0b, 0x14, 0x10, 0x10, 0x10, 0x10, 0x08, 0x0c, 0x07, 0x04, 0x01,
        0x00, 0x10, 0x18, 0x14, 0x12, 0x15, 0x12, 0x19, 0x00, 0x03, 0x04, 0x0b, 0x14, 0x10, 0x10, 0x10,
        0x10, 0x08, 0x04, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        oled_write_raw_P(hdo_logo, sizeof(hdo_logo));
}

#    endif
// clang-format on

void render_status_main(void) {
    /* Show Keyboard Layout  */
    render_default_layer_state();
    render_keylock_status(host_keyboard_leds());
    render_bootmagic_status();
    render_user_status();

    render_keylogger_status();
}

void render_status_secondary(void) {
    /* Show Keyboard Layout  */
    render_logo();
    render_default_layer_state();
    render_layer_state();
    render_mod_status(get_mods() | get_oneshot_mods());

    render_keylogger_status();
}

void oled_task_user(void) {
//     if (timer_elapsed32(oled_timer) > 10000) {
//         oled_off();
//         return;
//     }
// #    ifndef SPLIT_KEYBOARD
//     else {
//         oled_on();
//     }
// #    endif
    update_log();
    if (is_master) {
        render_status_main();  // Renders the current keyboard state (layer, lock, caps, scroll, etc)
    } else {
#    ifdef SPLIT_TRANSPORT_MIRROR
        render_status_secondary();
#    else
        render_hdo_logo();
#    endif
    }
}

#endif

#ifdef ENCODER_ENABLE
void encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        // Volume control
        if (clockwise) {
            tap_code(KC_VOLD);
        } else {
            tap_code(KC_VOLU);
        }
    } else if (index == 1) {
        // Page up/Page down
        if (clockwise) {
            tap_code(KC_PGUP);
        } else {
            tap_code(KC_PGDN);
        }
    }
}
#endif
