#include "FatFsSpiOutputStream.h"
#include <pico/printf.h>
#include <string>

#ifdef int32_t
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

void FatFsSpiOutputStream::open(const char* name) {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) {
      DBG_PRINTF("Failed to mount SD card\n");
      return;
    }
  }
  
  DBG_PRINTF("openning file %s for write \n", name);
  _fr = f_open(&_fil, name, FA_WRITE|FA_OPEN_ALWAYS);
  if (FR_OK != _fr && FR_EXIST != _fr) {
    printf("f_open(%s) error: %s (%d)\n", name, FRESULT_str(_fr), _fr);
  }
  else {
    DBG_PRINTF("openned file %s ok!\n", name);
    _open = true;
    _fr = FR_OK;
  }
}

FatFsSpiOutputStream::FatFsSpiOutputStream(SdCardFatFsSpi* sdCard, const char* name) :
  _sdCard(sdCard),
  _open(false)
{
  open(name);
}

FatFsSpiOutputStream::FatFsSpiOutputStream(SdCardFatFsSpi* sdCard, const char* folder, const char* file) {
  std::string fname(folder);
  fname.append("/");
  fname.append(file);
  open(fname.c_str());
}

int32_t FatFsSpiOutputStream::write(uint8_t* buffer, const uint32_t length) {
  if (!_open) return -1;
  
  // TODO Handle errors with separate codes
  if (FR_OK != _fr) return -1;

  UINT bw = 0;
  _fr = f_write(&_fil, buffer, length, &bw);
  if (_fr != FR_OK) {
    DBG_PRINTF("f_write(%s) error: (%d)\n", FRESULT_str(_fr), _fr);
    return -2;
  }
  if (bw < length) {
    DBG_PRINTF("f_write error: wrote %d of %d\n", bw, length);
    return -3; // failed to write
  }
  return bw;
}

void FatFsSpiOutputStream::close() {
  if (_open) {
    f_close(&_fil);
    _open = false;
  }
}

bool FatFsSpiOutputStream::closed() {
  return !_open;
}
