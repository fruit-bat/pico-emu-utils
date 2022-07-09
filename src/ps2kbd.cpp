// You may use, distribute and modify this code under the
// terms of the GPLv2 license, which unfortunately won't be
// written for another century.
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
#include "ps2kbd.h"
#include "ps2kbd.pio.h"
#include "hardware/clocks.h"

#define HID_KEYBOARD_REPORT_MAX_KEYS 6
/*
a A 07 04 1E 9E 1C F0 1C
b B 07 05 30 B0 32 F0 32
c C 07 06 2E AE 21 F0 21
d D 07 07 20 A0 23 F0 23
e E 07 08 12 92 24 F0 24
f F 07 09 21 A1 2B F0 2B
g G 07 0A 22 A2 34 F0 34
h H 07 0B 23 A3 33 F0 33
i I 07 0C 17 97 43 F0 43
j J 07 0D 24 A4 3B F0 3B
k K 07 0E 25 A5 42 F0 42
l L 07 0F 26 A6 4B F0 4B
m M 07 10 32 B2 3A F0 3A
n N 07 11 31 B1 31 F0 31
o O 07 12 18 98 44 F0 44
p P 07 13 19 99 4D F0 4D
q Q 07 14 10 90 15 F0 15
r R 07 15 13 93 2D F0 2D
s S 07 16 1F 9F 1B F0 1B
t T 07 17 14 94 2C F0 2C
u U 07 18 16 96 3C F0 3C
v V 07 19 2F AF 2A F0 2A
w W 07 1A 11 91 1D F0 1D
x X 07 1B 2D AD 22 F0 22
y Y 07 1C 15 95 35 F0 35
z Z 07 1D 2C AC 1A F0 1A
1 ! 07 1E 02 82 16 F0 16
2 @ 07 1F 03 83 1E F0 1E
3 # 07 20 04 84 26 F0 26
4 $ 07 21 05 85 25 F0 25
5 % 07 22 06 86 2E F0 2E
6 ^ 07 23 07 87 36 F0 36
7 & 07 24 08 88 3D F0 3D
8 * 07 25 09 89 3E F0 3E
9 ( 07 26 0A 8A 46 F0 46
0 ) 07 27 0B 8B 45 F0 45
Return 07 28 1C 9C 5A F0 5A
Escape 07 29 01 81 76 F0 76
Backspace 07 2A 0E 8E 66 F0 66
Tab 07 2B 0F 8F 0D F0 0D
Space 07 2C 39 B9 29 F0 29
- _ 07 2D 0C 8C 4E F0 4E
= + 07 2E 0D 8D 55 F0 55
[ { 07 2F 1A 9A 54 F0 54
] } 07 30 1B 9B 5B F0 5B
\ | 07 31 2B AB 5D F0 5D
Europe 1 (Note 2) 07 32 2B AB 5D F0 5D
; : 07 33 27 A7 4C F0 4C
' " 07 34 28 A8 52 F0 52
` ~ 07 35 29 A9 0E F0 0E
, < 07 36 33 B3 41 F0 41
. > 07 37 34 B4 49 F0 49
/ ? 07 38 35 B5 4A F0 4A
Caps Lock 07 39 3A BA 58 F0 58
F1 07 3A 3B BB 05 F0 05
F2 07 3B 3C BC 06 F0 06
F3 07 3C 3D BD 04 F0 04
F4 07 3D 3E BE 0C F0 0C
F5 07 3E 3F BF 03 F0 03
F6 07 3F 40 C0 0B F0 0B
F7 07 40 41 C1 83 F0 83
F8 07 41 42 C2 0A F0 0A
F9 07 42 43 C3 01 F0 01
F10 07 43 44 C4 09 F0 09
F11 07 44 57 D7 78 F0 78
F12 07 45 58 D8 07 F0 07 

*/

Ps2Kbd::Ps2Kbd(PIO pio, uint base_gpio) :
  _pio(pio),
  _base_gpio(base_gpio),
  _release(false),
  _keys_pressed(0)
{
  _report.modifier = 0;
  for (int i = 0; i < HID_KEYBOARD_REPORT_MAX_KEYS; ++i) _report.keycode[i] = 0;
}

void Ps2Kbd::tick() {

  while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) {
    // pull a scan code from the PIO SM fifo
    uint8_t code = *((io_rw_8*)&_pio->rxf[_sm] + 3);    
    printf("PS/2 keycode %d\n", code);

    // TODO is 170 (decimal) a reset code
    
    // TODO 0xe0 also takes a second code
    // E0 37
    // E0 F0 37
    // need to cope with these even if they are not translated
    
    // TODO 0xe1 looks like it takes two extra codes
    // Pause E1 14 77 
    // and never get released!
    // An 0xe1 code can be followed by an 0xf0 but this is not a release code
    
 
    if (code == 0xf0) {
      _release = true;
    }
    else {
      if (_release) {
        // process a key release
        printf("PS/2 key release %d\n", code);

      }
      else {
        // process a key press
        printf("PS/2 key press %d\n", code);

      }
      
      _release = false;
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
