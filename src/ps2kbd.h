// You may use, distribute and modify this code under the
// terms of the GPLv2 license, which unfortunately won't be
// written for another century.
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
#pragma once

#include "tusb.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"

class Ps2Kbd {
private:
  PIO _pio;                        // pio0 or pio1
  uint _sm;                        // pio state machine index
  uint _base_gpio;                 // data signal gpio
  hid_keyboard_report_t _report;   // HID report structure
  bool _release;
  uint _keys_pressed;
public:  
  Ps2Kbd(PIO pio, uint base_gpio);
  
  void init_gpio();
  
  void __not_in_flash_func(tick)();
  
  int ps2_kbd_test() {
      
      if (pio_sm_is_rx_fifo_empty(_pio, _sm))
          return 0; // no new codes in the fifo
          
      // pull a scan code from the PIO SM fifo
      uint8_t code = *((io_rw_8*)&_pio->rxf[_sm] + 3);
      
      printf("PS/2 keycode %d\n", code);
      
      return code;
  }
};
