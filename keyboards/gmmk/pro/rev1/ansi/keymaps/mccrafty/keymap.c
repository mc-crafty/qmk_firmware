/* Copyright 2022 MC Crafty
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

enum userspace_layers {
  QWERTY,
  FNLAYER
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // The FN key by default maps to a momentary toggle to layer 1 to provide access to the QK_BOOT key (to put the board into bootloader mode). Without
    // this mapping, you have to open the case to hit the button on the bottom of the PCB (near the USB cable attachment) while plugging in the USB
    // cable to get the board into bootloader mode - definitely not fun when you're working on your QMK builds. Remove this and put it back to KC_RGUI
    // if that's your preference.
    //
    // To put the keyboard in bootloader mode, use FN+backslash. If you accidentally put it into bootloader, you can just unplug the USB cable and
    // it'll be back to normal when you plug it back in.

    [QWERTY] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_PSCR,          KC_MEDIA_PLAY_PAUSE,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,          KC_DEL,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_PGUP,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,           KC_PGDN,
        KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, KC_UP,   KC_END,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                       KC_RALT, MO(FNLAYER),KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [FNLAYER] = LAYOUT(
        _______, KC_MYCM, KC_WHOM, KC_CALC, KC_MSEL, KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP, KC_MUTE, KC_VOLD, KC_VOLU, _______, KC_SCRL,          KC_MUTE,
        _______,  RGB_TOG, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          KC_INS,
        _______,  _______, RGB_VAI, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, QK_BOOT,          KC_PSLS,
        _______,  _______, RGB_VAD, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,          KC_PAST,
        _______,           _______, RGB_HUI, _______, _______, _______, NK_TOGG, _______, _______, _______, _______,          _______, RGB_MOD, KC_HOME,
        _______,  _______, _______,                            _______,                            _______, _______, _______, RGB_SPD, RGB_RMOD, RGB_SPI
    ),
    
    
};

#if defined(ENCODER_ENABLE) && !defined(ENCODER_DEFAULTACTIONS_ENABLE) // Encoder Functionality when not using userspace defaults
bool encoder_update_user(uint8_t index, bool clockwise) {
  if(IS_LAYER_ON(FNLAYER)) {
    if (clockwise) {
      tap_code(KC_MEDIA_NEXT_TRACK);
    } else {
      tap_code(KC_MEDIA_PREV_TRACK);
    }
    return false;
  }
  // custom mod handling for old numkey hotkeys
  uint8_t curr_mod = get_mods();

  if (curr_mod & MOD_MASK_CTRL) {
    if (clockwise) {
      tap_code16(KC_KP_3);
    } else {
      tap_code16(KC_KP_1);
    }
      return false;
  }
  if (curr_mod & MOD_MASK_ALT) {
    del_mods(curr_mod);
    if (clockwise) {
      tap_code16(LCTL(KC_KP_0));
    } else {
      tap_code16(LCTL(KC_KP_DOT));
    }
    set_mods(curr_mod);
    return false;
  }
  if (curr_mod & MOD_MASK_SHIFT) {
    del_mods(curr_mod);
    if (clockwise) {
      tap_code16(LCTL(KC_KP_PLUS));
    } else {
      tap_code16(LCTL(KC_KP_MINUS));
    }
    set_mods(curr_mod);
    return false;
  }

  if (clockwise) {
    tap_code(KC_VOLU);
  } else {
    tap_code(KC_VOLD);
  }
  return false;
}
#endif // ENCODER_ENABLE && !ENCODER_DEFAULTACTIONS_ENABLE

#ifdef RGB_MATRIX_ENABLE
int caps_lock_leds[] = {1, 2, 3, 4, 5, 67, 70, 73, 76, 80,  83, 87, 91};
int caps_lock_leds_size = sizeof caps_lock_leds / sizeof caps_lock_leds[0];

void keyboard_post_init_user(void) {
  rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
  rgb_matrix_set_color_all(RGB_CYAN); // Default startup colour
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
  if (host_keyboard_led_state().scroll_lock) {
    rgb_matrix_set_color_all(RGB_RED); //printscreen (fn_layer scroll lock) key
  } else {
    rgb_matrix_set_color_all(RGB_CYAN); // Default startup colour
    if (host_keyboard_led_state().caps_lock) {
      for (int i = 0; i < caps_lock_leds_size; i++) {
        rgb_matrix_set_color(caps_lock_leds[i], RGB_RED);
      }
    }
    if(IS_LAYER_ON(FNLAYER)) {
      rgb_matrix_set_color(0, RGB_RED); // esc changes based on fn
    }
  }
  return false;
}

void suspend_power_down_user(void) {
  rgb_matrix_set_suspend_state(true);
}

void suspend_wakeup_init_user(void) {
  rgb_matrix_set_suspend_state(false);
}
#endif
