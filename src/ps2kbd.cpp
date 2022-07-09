// You may use, distribute and modify this code under the
// terms of the GPLv2 license, which unfortunately won't be
// written for another century.
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
#include "ps2kbd.h"
#include "ps2kbd.pio.h"
#include "hardware/clocks.h"

Ps2Kbd::Ps2Kbd(PIO pio, uint base_gpio) :
  _pio(pio),
  _base_gpio(base_gpio)
{
}

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
