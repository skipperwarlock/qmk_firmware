#include QMK_KEYBOARD_H
#include "debug.h"
#include "action_layer.h"
#include "version.h"
#include "st7565.h"

#define BASE 0 // default layer
#define NUMB 1 // number pad
#define MDIA 2 // media keys

// Simple HSV to RGB conversion function
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} simple_rgb_t;

simple_rgb_t hsv_to_rgb_simple(uint8_t h, uint8_t s, uint8_t v) {
    simple_rgb_t rgb = {0, 0, 0};
    uint8_t region, remainder, p, q, t;

    if (s == 0) {
        rgb.r = v;
        rgb.g = v;
        rgb.b = v;
        return rgb;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0:
            rgb.r = v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = v;
            break;
        default:
            rgb.r = v; rgb.g = p; rgb.b = q;
            break;
    }

    return rgb;
}

enum custom_keycodes {
  PLACEHOLDER = SAFE_RANGE, // can always be here
  EPRM,
  VRSN,
  RGB_SLD
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Keymap 0: Basic layer
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * |   `    |   1  |   2  |   3  |   4  |   5  | DEL  |           |PrnScr|   6  |   7  |   8  |   9  |   0  |   -    |
 * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
 * | Del    |   Q  |   W  |   E  |   R  |   T  |      |           |      |   Y  |   U  |   I  |   O  |   P  |   \    |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * | LCtrl  |Gui/A |Alt/S |Sft/D |Ctl/F |   G  |------|           |------|   H  |Ctl/J |Sft/K |Alt/L |; / L2|' / Win |
 * |--------+------+------+------+------+------| LAlt |           |      |------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  |      |           |      |   N  |   M  |   ,  |   .  |  /   | RShift |
 * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
 *   | Win  |      |      |      | ~L1  |                                       |      |      |      |      | ~L1  |
 *   `----------------------------------'                                       `----------------------------------'
 *                                        ,-------------.       ,-------------.
 *                                        | Tab  |      |       |      |RCtl/Esc|
 *                                 ,------|------|------|       |------+--------+------.
 *                                 |      |      | Home |       | PgUp |        |      |
 *                                 | Space|Backsp|------|       |------| Enter  |Space |
 *                                 |      |ace   | End  |       | PgDn |        |      |
 *                                 `--------------------'       `----------------------'
 */
// If it accepts an argument (i.e, is a function), it doesn't need KC_.
// Otherwise, it needs KC_*
[BASE] = LAYOUT_ergodox(  // layer 0 : default
        // left hand
        KC_GRV,        KC_1,         KC_2,   KC_3,   KC_4,   KC_5,   KC_DEL,
        KC_TAB,        KC_Q,         KC_W,   KC_E,   KC_R,   KC_T,   KC_TRNS,
        KC_LCTL,       GUI_T(KC_A),  LALT_T(KC_S), LSFT_T(KC_D), LCTL_T(KC_F), KC_G,
        KC_LSFT,       KC_Z,         KC_X,   KC_C,   KC_V,   KC_B,   KC_LALT,
        KC_LGUI,	   KC_TRNS,      KC_TRNS,KC_TRNS,MO(NUMB),
													 KC_TAB, KC_TRNS,
                                                             KC_HOME,
                                              KC_SPC,KC_BSPC,KC_END,
        // right hand
             KC_PSCR,    KC_6,   KC_7,   KC_8,   KC_9,   KC_0,             KC_MINS,
             KC_TRNS,    KC_Y,   KC_U,   KC_I,   KC_O,   KC_P,             KC_BSLS,
                         KC_H,   LCTL_T(KC_J), LSFT_T(KC_K), LALT_T(KC_L), LT(MDIA, KC_SCLN), GUI_T(KC_QUOT),
             KC_TRNS,	 KC_N,   KC_M,   KC_COMM,KC_DOT, KC_SLSH, 		   KC_RSFT,
                                 KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,          TT(NUMB),
             KC_TRNS,RCTL_T(KC_ESC),
             KC_PGUP,
             KC_PGDN,KC_ENT, KC_SPC
    ),
/* Keymap 1: Symbol Layer (NUMB)
 *
 * ,---------------------------------------------------.           ,--------------------------------------------------.
 * | (none)  |(none)|(none)|(none)|(none)|(none)|      |           |      |  F6  |  F7  |  F8  |  F9  |  F10 |   F11  |
 * |---------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * | (none)  |(none)|(none)|(none)|(none)|(none)|      |           |      |   Up |   7  |   8  |   9  |   *  |   F12  |
 * |---------+------+------+------+------+-------------|           |------+------+------+------+------+---------------|
 * | LCtrl   | LGui | LAlt |LShift| LCtrl|(none)|      |           |      | Down |   4  |   5  |   6  |   +  |        |
 * |---------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * | (none)  |(none)|(none)|(none)|(none)|(none)|      |           |      |   &  |   1  |   2  |   3  |   \  |        |
 * `---------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
 *   |       |      |      |      |      |                                       |      |   0  |   .  |   =  |      |
 *   `-----------------------------------'                                       `----------------------------------'
 *                                        ,-------------.       ,-------------.
 *                                        |      |      |       |Toggle|      |
 *                                 ,------|------|------|       |------+------+------.
 *                                 |      |      |      |       |INC   |      |      |
 *                                 |      |      |------|       |------|      |      |
 *                                 |      |      |      |       |DEC   |      |      |
 *                                 `--------------------'       `--------------------'
 */
// Number Pad — left: plain modifiers on home row (same roles as base); rest blank
[NUMB] = LAYOUT_ergodox(
       // left hand
       KC_NO,    KC_NO, KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,
       KC_NO,    KC_NO, KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,
       KC_LCTL,  KC_LGUI, KC_LALT, KC_LSFT, KC_LCTL, KC_NO,
       KC_NO,    KC_NO, KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,
          KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,
                                       KC_TRNS,KC_TRNS,
                                               KC_TRNS,
                               KC_TRNS,KC_TRNS,KC_TRNS,
       // right hand
       KC_TRNS, KC_F6,   KC_F7,  KC_F8,   KC_F9,   KC_F10,  KC_F11,
       KC_TRNS, KC_UP,   KC_7,   KC_8,    KC_9,    KC_ASTR, KC_F12,
                KC_DOWN, KC_4,   KC_5,    KC_6,    KC_PLUS, KC_TRNS,
       KC_TRNS, KC_AMPR, KC_1,   KC_2,    KC_3,    KC_BSLS, KC_TRNS,
                         KC_TRNS,KC_0,    KC_DOT,  KC_EQL,  KC_TRNS,
       LM_TOGG, KC_TRNS,
       LM_BRIU,
       LM_BRID, KC_TRNS, KC_TRNS
),
/* Keymap 2: Media and mouse keys
 *
 * ,--------------------------------------------------.           ,--------------------------------------------------.
 * |        |      |      |      |      |      |      |           |      |      |      |      |      |      |        |
 * |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
 * |        |      |      | MsUp |   =  |      |      |           |      |      |  [   |  Up  |  ]   |      |        |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * |        |LShift|MsLeft|MsDown|MsRght| Home |------|           |------|      | Left | Down |Right |      |        |
 * |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
 * |        |      |      |      |      | End  |      |           |      |      |      |      |      |      |        |
 * `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
 *   |      |      |      |      |      |                                       |VolUp |VolDn | Mute |      |      |
 *   `----------------------------------'                                       `----------------------------------'
 *                                        ,-------------.       ,-------------.
 *                                        |      |      |       |      |      |
 *                                 ,------|------|------|       |------+------+------.
 *                                 |      |      |      |       |      |      |      |
 *                                 | Lclk | Rclk |------|       |------|      |      |
 *                                 |      |      |      |       | Home |      |      |
 *                                 `--------------------'       `--------------------'
 */
// MEDIA AND MOUSE
[MDIA] = LAYOUT_ergodox(
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS, KC_TRNS, KC_TRNS, KC_MS_U, KC_EQL,  KC_TRNS, KC_TRNS,
       KC_TRNS, KC_LSFT, KC_MS_L, KC_MS_D, KC_MS_R, KC_HOME,
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_END, KC_TRNS,
       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                           KC_TRNS, KC_TRNS,
                                                    KC_TRNS,
                                  KC_BTN1, KC_BTN2, KC_TRNS,
    // right hand
       KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
       KC_TRNS,  KC_TRNS, KC_LBRC, KC_UP,   KC_RBRC, KC_TRNS, KC_TRNS,
                 KC_TRNS, KC_LEFT, KC_DOWN, KC_RIGHT,KC_TRNS, KC_TRNS,
       KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                          KC_VOLU, KC_VOLD, KC_MUTE, KC_TRNS, KC_TRNS,
       KC_TRNS, KC_TRNS,
       KC_TRNS,
       KC_HOME, KC_TRNS, KC_TRNS
),
};

// Helper function to get layer name
const char* get_layer_name(uint8_t layer) {
    switch (layer) {
        case BASE:
            return "Qwerty";
        case NUMB:
            return "Numpad";
        case MDIA:
            return "Mouse/Media";
        default:
            return "Unknown";
    }
}

// Helper function to update LCD backlight color based on layer
void update_lcd_color_for_layer(uint8_t layer) {
    // Define color hue values (from old visualizer)
    uint8_t hue;
    switch (layer) {
        case BASE:  // Qwerty - OCEAN (cyan/teal)
            hue = 149;
            break;
        case NUMB:  // Numpad - SPRING_GREEN (bright green)
            hue = 64;
            break;
        case MDIA:  // Media/Mouse - RED
            hue = 0;
            break;
        default:
            hue = 149;  // Default to OCEAN
            break;
    }
    
    // Convert HSV to RGB
    simple_rgb_t rgb = hsv_to_rgb_simple(hue, 255, 255);
    
    // Scale RGB values to 16-bit range for ergodox_infinity_lcd_color
    // The function expects values 0-65535 (uint16_t)
    ergodox_infinity_lcd_color(rgb.r << 8, rgb.g << 8, rgb.b << 8);
}

// Layer state callback to change LCD backlight color (master half)
layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);
    update_lcd_color_for_layer(layer);
    return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    // dynamically generate these.
    case EPRM:
      if (record->event.pressed) {
        eeconfig_init();
      }
      return false;
      break;
    case VRSN:
      if (record->event.pressed) {
        SEND_STRING (QMK_KEYBOARD "/" QMK_KEYMAP " @ " QMK_VERSION);
      }
      return false;
      break;
    case RGB_SLD:
      if (record->event.pressed) {
        #ifdef RGBLIGHT_ENABLE
          rgblight_mode(1);
        #endif
      }
      return false;
      break;
  }
  return true;
}

// Runs just one time when the keyboard initializes.
void matrix_init_user(void) {

};


// Runs constantly in the background, in a loop.
void matrix_scan_user(void) {
    static uint8_t last_layer = 0xFF;
    uint8_t layer = biton32(layer_state);

    // Update LCD color on both halves when layer changes
    if (layer != last_layer) {
        last_layer = layer;
        update_lcd_color_for_layer(layer);
    }

    ergodox_board_led_off();
    ergodox_right_led_1_off();
    ergodox_right_led_2_off();
    ergodox_right_led_3_off();
    switch (layer) {
      // TODO: Make this relevant to the ErgoDox EZ.
        case 1:
            ergodox_right_led_1_on();
            break;
        case 2:
            ergodox_right_led_2_on();
            break;
        default:
            // none
            break;
    }

};

// Override LCD display to show custom layer text and eliminate flicker
void st7565_task_user(void) {
    static uint8_t last_layer_master = 0xFF;  // Track last layer on master
    uint8_t current_layer = get_highest_layer(layer_state);
    
    if (is_keyboard_master()) {
        // Master half: Show custom layer text
        // Only redraw if layer changed (eliminates flicker)
        if (current_layer != last_layer_master) {
            last_layer_master = current_layer;
            
            // Clear the display
            st7565_clear();
            
            // Line 1: Show keyboard LED status
            st7565_set_cursor(0, 0);
            led_t leds = host_keyboard_led_state();
            if (leds.num_lock) { st7565_write("Num ", false); }
            if (leds.caps_lock) { st7565_write("Cap ", false); }
            if (leds.scroll_lock) { st7565_write("Scrl ", false); }
            if (leds.compose) { st7565_write("Com ", false); }
            if (leds.kana) { st7565_write("Kana", false); }
            st7565_advance_page(true);
            
            // Line 2: Show current layer name
            st7565_set_cursor(0, 1);
            st7565_write("Layer: ", false);
            st7565_write_ln(get_layer_name(current_layer), false);
            
            // Line 3: Blank line for spacing
            st7565_advance_page(true);
            
            // Line 4: Show layer number
            st7565_set_cursor(0, 3);
            char layer_str[16];
            snprintf(layer_str, sizeof(layer_str), "Layer %d", current_layer);
            st7565_write_ln(layer_str, false);
        }
    } else {
        // Slave half: Show layer info only
        // Force update on first call or when layer changes
        static bool initialized = false;  // Track if we've drawn at least once
        if (!initialized || current_layer != last_layer_master) {
            // On first init, try setting a bright test color (full red)
            if (!initialized) {
                ergodox_infinity_lcd_color(0xFFFF, 0, 0);  // Full red for testing
            }
            
            initialized = true;
            last_layer_master = current_layer;
            
            // Update LCD backlight color immediately
            update_lcd_color_for_layer(current_layer);
            
            // Clear screen completely
            st7565_clear();
            
            // Show layer information
            st7565_set_cursor(0, 0);
            st7565_write("Layer: ", false);
            st7565_write_ln(get_layer_name(current_layer), false);
            
            st7565_set_cursor(0, 1);
            char layer_str[16];
            snprintf(layer_str, sizeof(layer_str), "Layer %d", current_layer);
            st7565_write_ln(layer_str, false);
        }
    }
}

