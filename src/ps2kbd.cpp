// You may use, distribute and modify this code under the
// terms of the GPLv2 license, which unfortunately won't be
// written for another century.
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// See:
// http://www.vetra.com/scancodes.html
// https://wiki.osdev.org/PS/2_Keyboard
//
#include "ps2kbd.h"
#include "ps2kbd.pio.h"
#include "hardware/clocks.h"

#define HID_KEYBOARD_REPORT_MAX_KEYS 6

static uint8_t ps2kbd_page_0[] {
  /* 00 (  0) */ HID_KEY_NONE,
  /* 01 (  1) */ HID_KEY_F9,
  /* 02 (  2) */ 0x00,
  /* 03 (  3) */ HID_KEY_F5,
  /* 04 (  4) */ HID_KEY_F3,
  /* 05 (  5) */ HID_KEY_F1,
  /* 06 (  6) */ HID_KEY_F2,
  /* 07 (  7) */ HID_KEY_F12,
  /* 08 (  8) */ 0x00,
  /* 09 (  9) */ HID_KEY_F10,
  /* 0A ( 10) */ HID_KEY_F8,
  /* 0B ( 11) */ HID_KEY_F6,
  /* 0C ( 12) */ HID_KEY_F4,
  /* 0D ( 13) */ HID_KEY_TAB,
  /* 0E ( 14) */ HID_KEY_GRAVE,
  /* 0F ( 15) */ 0x00,
  /* 10 ( 16) */ 0x00,
  /* 11 ( 17) */ 0x00,
  /* 12 ( 18) */ 0x00,
  /* 13 ( 19) */ 0x00,
  /* 14 ( 20) */ 0x00,
  /* 15 ( 21) */ HID_KEY_Q,
  /* 16 ( 22) */ HID_KEY_1,
  /* 17 ( 23) */ 0x00,
  /* 18 ( 24) */ 0x00,
  /* 19 ( 25) */ 0x00,
  /* 1A ( 26) */ HID_KEY_Z,
  /* 1B ( 27) */ HID_KEY_S,
  /* 1C ( 28) */ HID_KEY_A,
  /* 1D ( 29) */ HID_KEY_W,
  /* 1E ( 30) */ HID_KEY_2,
  /* 1F ( 31) */ 0x00,
  /* 20 ( 32) */ 0x00,
  /* 21 ( 33) */ HID_KEY_C,
  /* 22 ( 34) */ HID_KEY_X,
  /* 23 ( 35) */ HID_KEY_D,
  /* 24 ( 36) */ HID_KEY_E,
  /* 25 ( 37) */ HID_KEY_4,
  /* 26 ( 38) */ HID_KEY_3,
  /* 27 ( 39) */ 0x00,
  /* 28 ( 40) */ 0x00,
  /* 29 ( 41) */ HID_KEY_SPACE,
  /* 2A ( 42) */ HID_KEY_V,
  /* 2B ( 43) */ HID_KEY_F,
  /* 2C ( 44) */ HID_KEY_T,
  /* 2D ( 45) */ HID_KEY_R,
  /* 2E ( 46) */ HID_KEY_5,
  /* 2F ( 47) */ 0x00,
  /* 30 ( 48) */ 0x00,
  /* 31 ( 49) */ HID_KEY_N,
  /* 32 ( 50) */ HID_KEY_B,
  /* 33 ( 51) */ HID_KEY_H,
  /* 34 ( 52) */ HID_KEY_G,
  /* 35 ( 53) */ HID_KEY_Y,
  /* 36 ( 54) */ HID_KEY_6,
  /* 37 ( 55) */ 0x00,
  /* 38 ( 56) */ 0x00,
  /* 39 ( 57) */ 0x00,
  /* 3A ( 58) */ HID_KEY_M,
  /* 3B ( 59) */ HID_KEY_J,
  /* 3C ( 60) */ HID_KEY_U,
  /* 3D ( 61) */ HID_KEY_7,
  /* 3E ( 62) */ HID_KEY_8,
  /* 3F ( 63) */ 0x00,
  /* 40 ( 64) */ 0x00,
  /* 41 ( 65) */ HID_KEY_COMMA,
  /* 42 ( 66) */ HID_KEY_K,
  /* 43 ( 67) */ HID_KEY_I,
  /* 44 ( 68) */ HID_KEY_O,
  /* 45 ( 69) */ HID_KEY_0,
  /* 46 ( 70) */ HID_KEY_9,
  /* 47 ( 71) */ 0x00,
  /* 48 ( 72) */ 0x00,
  /* 49 ( 73) */ HID_KEY_PERIOD,
  /* 4A ( 74) */ HID_KEY_SLASH,
  /* 4B ( 75) */ HID_KEY_L,
  /* 4C ( 76) */ HID_KEY_SEMICOLON,
  /* 4D ( 77) */ HID_KEY_P,
  /* 4E ( 78) */ HID_KEY_MINUS,
  /* 4F ( 79) */ 0x00,
  /* 50 ( 80) */ 0x00,
  /* 51 ( 81) */ 0x00,
  /* 52 ( 82) */ HID_KEY_APOSTROPHE,
  /* 53 ( 83) */ 0x00,
  /* 54 ( 84) */ HID_KEY_BRACKET_LEFT,
  /* 55 ( 85) */ HID_KEY_EQUAL,
  /* 56 ( 86) */ 0x00,
  /* 57 ( 87) */ 0x00,
  /* 58 ( 88) */ HID_KEY_CAPS_LOCK,
  /* 59 ( 89) */ 0x00,
  /* 5A ( 90) */ HID_KEY_ENTER, // RETURN ??
  /* 5B ( 91) */ HID_KEY_BRACKET_RIGHT,
  /* 5C ( 92) */ 0x00,
  /* 5D ( 93) */ HID_KEY_BACKSLASH,
  /* 5E ( 94) */ 0x00,
  /* 5F ( 95) */ 0x00,
  /* 60 ( 96) */ 0x00,
  /* 61 ( 97) */ 0x00,
  /* 62 ( 98) */ 0x00,
  /* 63 ( 99) */ 0x00,
  /* 64 (100) */ 0x00,
  /* 65 (101) */ 0x00,
  /* 66 (102) */ HID_KEY_BACKSPACE,
  /* 67 (103) */ 0x00,
  /* 68 (104) */ 0x00,
  /* 69 (105) */ 0x00,
  /* 6A (106) */ 0x00,
  /* 6B (107) */ 0x00,
  /* 6C (108) */ 0x00,
  /* 6D (109) */ 0x00,
  /* 6E (110) */ 0x00,
  /* 6F (111) */ 0x00,
  /* 70 (112) */ 0x00,
  /* 71 (113) */ 0x00,
  /* 72 (114) */ 0x00,
  /* 73 (115) */ 0x00,
  /* 74 (116) */ 0x00,
  /* 75 (117) */ 0x00,
  /* 76 (118) */ HID_KEY_ESCAPE,
  /* 77 (119) */ 0x00,
  /* 78 (120) */ HID_KEY_F11,
  /* 79 (121) */ 0x00,
  /* 7A (122) */ 0x00,
  /* 7B (123) */ 0x00,
  /* 7C (124) */ 0x00,
  /* 7D (125) */ 0x00,
  /* 7E (126) */ 0x00,
  /* 7F (127) */ 0x00,
  /* 80 (128) */ 0x00,
  /* 81 (129) */ 0x00,
  /* 82 (130) */ 0x00,
  /* 83 (131) */ HID_KEY_F7,
  /* 84 (132) */ 0x00,
  /* 85 (133) */ 0x00,
  /* 86 (134) */ 0x00,
  /* 87 (135) */ 0x00,
  /* 88 (136) */ 0x00,
  /* 89 (137) */ 0x00,
  /* 8A (138) */ 0x00,
  /* 8B (139) */ 0x00,
  /* 8C (140) */ 0x00,
  /* 8D (141) */ 0x00,
  /* 8E (142) */ 0x00,
  /* 8F (143) */ 0x00,
  /* 90 (144) */ 0x00,
  /* 91 (145) */ 0x00,
  /* 92 (146) */ 0x00,
  /* 93 (147) */ 0x00,
  /* 94 (148) */ 0x00,
  /* 95 (149) */ 0x00,
  /* 96 (150) */ 0x00,
  /* 97 (151) */ 0x00,
  /* 98 (152) */ 0x00,
  /* 99 (153) */ 0x00,
  /* 9A (154) */ 0x00,
  /* 9B (155) */ 0x00,
  /* 9C (156) */ 0x00,
  /* 9D (157) */ 0x00,
  /* 9E (158) */ 0x00,
  /* 9F (159) */ 0x00,
  /* A0 (160) */ 0x00,
  /* A1 (161) */ 0x00,
  /* A2 (162) */ 0x00,
  /* A3 (163) */ 0x00,
  /* A4 (164) */ 0x00,
  /* A5 (165) */ 0x00,
  /* A6 (166) */ 0x00,
  /* A7 (167) */ 0x00,
  /* A8 (168) */ 0x00,
  /* A9 (169) */ 0x00,
  /* AA (170) */ 0x00,
  /* AB (171) */ 0x00,
  /* AC (172) */ 0x00,
  /* AD (173) */ 0x00,
  /* AE (174) */ 0x00,
  /* AF (175) */ 0x00,
  /* B0 (176) */ 0x00,
  /* B1 (177) */ 0x00,
  /* B2 (178) */ 0x00,
  /* B3 (179) */ 0x00
};

Ps2Kbd::Ps2Kbd(PIO pio, uint base_gpio) :
  _pio(pio),
  _base_gpio(base_gpio),
  _double(false)
{
  _report.modifier = 0;
  for (int i = 0; i < HID_KEYBOARD_REPORT_MAX_KEYS; ++i) _report.keycode[i] = 0;
  
  clearActions();
}

uint8_t Ps2Kbd::hidCodePage0(uint8_t ps2code) {
  return ps2code < sizeof(ps2kbd_page_0) ? ps2kbd_page_0[ps2code] : HID_KEY_NONE;
}

uint8_t Ps2Kbd::hidCodePage1(uint8_t ps2code) {
  switch(ps2code) {
//  case 0x37: return HID_KEY_POWER;
//  case 0x3f: return HID_KEY_SLEEP;
//  case 0x5e: return HID_KEY_WAKE;
  case 0x7c: return HID_KEY_PRINT_SCREEN;
  case 0x70: return HID_KEY_INSERT;
  case 0x6c: return HID_KEY_HOME;
  case 0x7d: return HID_KEY_PAGE_UP;
  case 0x71: return HID_KEY_DELETE;
  case 0x69: return HID_KEY_END;
  case 0x7a: return HID_KEY_PAGE_DOWN;
  case 0x74: return HID_KEY_ARROW_RIGHT;
  case 0x6b: return HID_KEY_ARROW_LEFT;
  case 0x72: return HID_KEY_ARROW_DOWN;
  case 0x75: return HID_KEY_ARROW_UP;
  case 0x5a: return HID_KEY_KEYPAD_DIVIDE;

  default: 
    return HID_KEY_NONE;
  }
}

void Ps2Kbd::handleActions() {
  for (uint i = 0; i <= _action; ++i) {
    printf("PS/2 key %s page %2.2X (%3.3d) code %2.2X (%3.3d)\n",
      _actions[i].release ? "release" : "press",
      _actions[i].page,
      _actions[i].page,
      _actions[i].code,
      _actions[i].code);
  }
  
  uint8_t hidCode;
  bool release;
  if (_action == 0) {
    switch (_actions[0].page) {
      case 1: {
        hidCode = hidCodePage1(_actions[0].code);
        break; 
      }
      default: {
        hidCode = hidCodePage0(_actions[0].code);
        break;
      }
    }
    release = _actions[0].release;
  }
  else {
    // TODO get the HID code for extended PS/2 codes
    hidCode = HID_KEY_NONE;
    release = false;
  }
  
  if (hidCode != HID_KEY_NONE) {
    
    printf("HID key %s code %2.2X (%3.3d)\n",
      release ? "release" : "press",
      hidCode,
      hidCode);   
    
  }
}

void Ps2Kbd::tick() {

  while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) {
    // pull a scan code from the PIO SM fifo
    uint8_t code = *((io_rw_8*)&_pio->rxf[_sm] + 3);    
    printf("PS/2 keycode %2.2X (%d)\n", code, code);

    switch (code) {
      case 0xaa: {
         printf("PS/2 keyboard Self test passed\n");
         break;       
      }
      case 0xe1: {
        _double = true;
        break;
      }
      case 0xe0: {
        _actions[_action].page = 1;
        break;
      }
      case 0xf0: {
        _actions[_action].release = true;
        break;
      }
      default: {
        _actions[_action].code = code;
        if (_double) {
          _action = 1;
          _double = false;
        }
        else {
          handleActions();
          clearActions();
        }
        break;
      }
    }
  }
}

// TODO Error checking and reporting
void Ps2Kbd::init_gpio() {
    // init KBD pins to input
    gpio_init(_base_gpio);     // Data
    gpio_init(_base_gpio + 1); // Clock
    // with pull up
    gpio_pull_up(_base_gpio);
    gpio_pull_up(_base_gpio + 1);
    // get a state machine
    _sm = pio_claim_unused_sm(_pio, true);
    // reserve program space in SM memory
    uint offset = pio_add_program(_pio, &ps2kbd_program);
    // Set pin directions base
    pio_sm_set_consecutive_pindirs(_pio, _sm, _base_gpio, 2, false);
    // program the start and wrap SM registers
    pio_sm_config c = ps2kbd_program_get_default_config(offset);
    // Set the base input pin. pin index 0 is DAT, index 1 is CLK
    sm_config_set_in_pins(&c, _base_gpio);
    // Shift 8 bits to the right, autopush enabled
    sm_config_set_in_shift(&c, true, true, 8);
    // Deeper FIFO as we're not doing any TX
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    // We don't expect clock faster than 16.7KHz and want no less
    // than 8 SM cycles per keyboard clock.
    float div = (float)clock_get_hz(clk_sys) / (8 * 16700);
    sm_config_set_clkdiv(&c, div);
    // Ready to go
    pio_sm_init(_pio, _sm, offset, &c);
    pio_sm_set_enabled(_pio, _sm, true);
}
