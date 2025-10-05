#include <pico/printf.h>

#include "SdCardFatFsSpi.h"
#include "ff.h"
#include "ff_util.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

SdCardFatFsSpi::SdCardFatFsSpi(int unit) :
  _unit(unit),
  _mounted(false)
{
}

bool SdCardFatFsSpi::mount() {
  if (mounted()) return true;
  DBG_PRINTF("Mounting SD card %d... ", _unit);

  // This is needed for using high GPIO for the SDCARD PIO
#ifdef SDCARD_PIO
#if SDCARD_PIN_SPI0_CS > 31 || SDCARD_PIN_SPI0_SCK > 31 || SDCARD_PIN_SPI0_MOSI > 31 || SDCARD_PIN_SPI0_MISO > 31
  pio_set_gpio_base(SDCARD_PIO, 16);
#endif
#endif

  FRESULT fr = f_mount(&fs, "", 1);
  if (FR_OK != fr) {
    DBG_PRINTF(" error: %s (%d)\n", FRESULT_str(fr), fr);
    _mounted = false;
  }
  else {
    printf("Ok\n");
    _mounted = true;
  }
  return mounted();
}

void SdCardFatFsSpi::unmount() {
  if (mounted()) {
    DBG_PRINTF("Unmounting SD card %d... ", _unit);
    f_unmount("");
    DBG_PRINTF("Ok\n");
    _mounted = false;
  }
}
