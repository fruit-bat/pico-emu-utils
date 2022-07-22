#include <pico/printf.h>

#include "SdCardFatFsSpi.h"
#include "ff.h"
#include "ff_util.h"
#include "pico/stdlib.h"

SdCardFatFsSpi::SdCardFatFsSpi(int unit) :
  _unit(unit),
  _mounted(false)
{
}

bool SdCardFatFsSpi::mount() {
  if (mounted()) return true;
  printf("Mounting SD card %d... ", _unit);
  FRESULT fr = f_mount(&fs, "", 1);
  if (FR_OK != fr) {
    printf(" error: %s (%d)\n", FRESULT_str(fr), fr);
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
    printf("Unmounting SD card %d... ", _unit);
    f_unmount("");
    printf("Ok\n");
  }
}
