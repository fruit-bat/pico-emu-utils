#pragma once

#include "ff.h"


class SdCardFatFsSpi {
  int _unit;
  bool _mounted;
//  sd_card_t *_sdcard;
  FATFS fs;
public:
// Only supports unit 0
  SdCardFatFsSpi(int unit);
  bool mount();
  void unmount();
  bool mounted() { return _mounted; }
};
